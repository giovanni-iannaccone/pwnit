#include <cstring>
#include <iostream>

#include <pwnit/core/config/config.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>

#include <toml++/impl/parser.hpp>

#define DEFAULT_VALUED_PARAM(name, default_value, type)             \
    do {                                                            \
        std::optional<type> value;                                  \
        this->name =                                                \
            (value = config[ #name ].value<type>())                 \
            ? *value                                                \
            : default_value;                                        \
    } while (0)

#define REQUIRED_PARAM(name, type) \
    do {                                                                \
        if (auto value = config[ #name ].value<type>())                 \
            this->name = *value;                                        \
        else                                                            \
            assert::fail(false, "Invalid configs, missing required param {}", # name); \
    } while (0)

namespace pwnit::config
{

const auto standard_config =
    "elf_var_name = \"exe\"\n"                                          \
    "libc_var_name = \"libc\"\n"                                        \
    "ld_var_name = \"ld\"\n"                                            \
    "remote = \"r\"\n"                                                  \
    "import_everything = true\n"                                        \
    "solve_file = \"solve.py\"\n"                                       \
    "template_file = \"" + (std::filesystem::path(std::getenv("HOME")) / ".config/pwnit/templates/default.py\"\n").string() +
    "libc_db = \"https://libc.rip\"\n\n"                                \
    "dangerous_functions = [\"execve\", \"gets\", \"mmap\", \"strcpy\", \"system\"]\n";
    
static void create_config()
{
    std::filesystem::create_directories(default_path.parent_path());
    std::ofstream file(default_path);

    file.write(
        standard_config.c_str(),
        static_cast<long>(standard_config.size())
    );
}
    
void Config::parse()
{
    try {
        auto &&config = toml::parse_file(default_path.string());

        DEFAULT_VALUED_PARAM(elf_var_name, "elf", std::string);
        DEFAULT_VALUED_PARAM(libc_var_name, "libc", std::string);
        DEFAULT_VALUED_PARAM(ld_var_name, "ld", std::string);
        DEFAULT_VALUED_PARAM(remote, "r", std::string);
        
        DEFAULT_VALUED_PARAM(solve_file, "solve.py", std::string);
        DEFAULT_VALUED_PARAM(libc_db, "https://libc.rip", std::string);

        REQUIRED_PARAM(template_file, std::string);
        
        this->dangerous_functions = config["dangerous_functions"].as_array();

    } catch (const std::exception& e) {
        console::error("Error in config file: {}", e.what());
        console::log("Reset it ? y/n ");

        char opt;
        std::cin >> opt;

        if (opt == 'Y' || opt == 'y') {
            create_config();
            this->parse();
        }
    }
}
    
}

