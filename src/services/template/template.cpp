#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>

#include <pwnit/commands.hpp>
#include <pwnit/core/config/config.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/file.hpp>
#include <pwnit/utils/string.hpp>

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

namespace pwnit::templates
{

constexpr string::fixed elf  = "{} = ELF(\"./{}\", checksec=True)\n"; 
constexpr string::fixed libc = "{} = ELF(\"./{}\", checksec=False)\n";
constexpr string::fixed ld   = "{} = ELF(\"./{}\", checksec=False)";

static inline
std::string ask_solve_file()
{
    console::log("Solve already exists, press enter to overwrite it or new filename: ");
    
    std::string solve;
    std::getline(std::cin, solve);
    return solve;
}

template <string::fixed Fmt>
constexpr std::string
get_binaries_pwntools_decl(const std::string &varname, const std::filesystem::path &path)
{
    return std::format("{}", std::format(Fmt, varname, path.filename().string()));
}
    
static inline
std::string format_binaries(const commands::StartOptions &opt)
{
    const auto &cfg = config::Config::instance();
    std::string binaries = "";
    
    if (!opt.elf.empty())
        binaries +=
            get_binaries_pwntools_decl<elf>(cfg.elf_var_name, opt.elf);

    if (!opt.libc.empty())
        binaries +=
            get_binaries_pwntools_decl<libc>(cfg.libc_var_name, opt.libc);
    
    if (!opt.ld.empty())
        binaries +=
            get_binaries_pwntools_decl<ld>(cfg.ld_var_name, opt.ld);

    return binaries;
}

static inline nlohmann::json
get_data_for_template(commands::StartOptions &opt)
{
    const auto &cfg = config::Config::instance();
    nlohmann::json data;

    data["binaries"] = format_binaries(opt);
    data["remote"] = cfg.remote;

    data["elf_var"] = cfg.elf_var_name;
    data["libc_var"] = cfg.libc_var_name;
    data["ld_var"] = cfg.ld_var_name;

    return data;
}
    
static inline
std::string parse_template(commands::StartOptions &opt)
{
    std::string template_file = !opt.template_file.empty()
        ? opt.template_file
        : [] () {const auto &cfg = config::Config::instance(); return cfg.template_file;} ();
    
    std::ifstream template_fd {template_file};

    assert::fail(
        template_fd.is_open(),
        "Could not open template file: {}", template_file
    );

    std::string py_template = utils::read_whole_file(template_fd);
    nlohmann::json data = get_data_for_template(opt);

    try {
        return inja::render(py_template, data);
    } catch (std::exception &e) {
        console::error("{}", e.what());
    } catch (...) {
        console::error("Unknown error in template rendering");
    }

    return "";
}

static inline
void write_template(const std::string &result)
{
    const auto &cfg = config::Config::instance();

    std::string solve_file;
    if (std::filesystem::exists(cfg.solve_file))
        solve_file = ask_solve_file();

    if (solve_file.empty())
        solve_file = cfg.solve_file;

    std::ofstream solve_fd {solve_file};
    assert::fail(
        solve_fd.is_open(),
        "Could not write solve file: {}", solve_file
    );

    solve_fd.write(
         result.data(),
         static_cast<std::streamsize>(result.size())
    );

    console::success("\nSuccessfully created {}", solve_file);
}
    
void write_solve(commands::StartOptions &opt)
{
    std::string result = parse_template(opt);
    if (result.empty())
        return;
    
    write_template(result);
}

}
