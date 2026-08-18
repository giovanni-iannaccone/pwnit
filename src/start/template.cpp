#include <filesystem>
#include <format>
#include <fstream>
#include <ios>

#include <pwnit/commands.hpp>
#include <pwnit/config/config.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/file.hpp>

#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

namespace pwnit::start
{

constexpr auto elf = "exe = ELF(\"{}\", checksec=True)"; 
constexpr auto libc = "libc = ELF(\"{}\", checksec=False)";
constexpr auto ld = "ld = ELF(\"{}\", checksec=False)";

static inline
std::string ask_solve_file()
{
    console::log("Solve already exists, press enter to overwrite it or new filename: ");
    
    std::string solve;
    std::getline(std::cin, solve);
    return solve;
}
    
static inline
std::string format_binaries(const commands::StartOptions &opt)
{
    std::string binaries = "";

    if (!opt.elf.empty())
        binaries += std::format("{}\n", std::format(elf, opt.elf));

    if (!opt.libc.empty())
        binaries += std::format("{}\n", std::format(libc, opt.libc));

    if (!opt.ld.empty())
        binaries += std::format("{}\n", std::format(ld, opt.ld));

    return binaries;
}
    
static inline
std::string parse_template(commands::StartOptions &opt)
{
    const auto &cfg = config::Config::instance();
    std::ifstream template_fd {cfg.template_file};

    assert::fail(
        template_fd.is_open(),
        "Could not open template file: {}", cfg.template_file
    );

    std::string py_template = utils::read_whole_file(template_fd);
    
    nlohmann::json data;
    data["binaries"] = format_binaries(opt);

    return inja::render(py_template, data);
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
    write_template(result);
}

}
