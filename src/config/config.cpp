#include <cstring>
#include <iostream>

#include <pwnit/config/config.hpp>
#include <pwnit/utils/console.hpp>

#include <toml++/impl/parser.hpp>

namespace pwnit::config
{

const auto standard_config =
    "solve_file = \"solve.py\"\n" \
    "template_file = \"" + (std::filesystem::path(std::getenv("HOME")) / ".config/pwnit/templates/default.py\"\n").string() +
    "libc_db = \"https://libc.rip/api/find\"\n\n" \
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
        if (auto value = config["solve_file"].value<std::string>())
            this->solve_file = *value;
        
        if (auto value = config["template_file"].value<std::string>())
            this->template_file = *value;
        
        if (auto value = config["libc_db"].value<std::string>())
            this->libc_db = *value;
        
        this->dangerous_functions = config["dangerous_functions"].as_array();

    } catch (...) {
        console::error("Could not open file {}", default_path.string());
        console::log("Create it ? y/n ");

        char opt;
        std::cin >> opt;

        if (opt == 'Y' || opt == 'y') {
            create_config();
            this->parse();
        }
    }
}
    
}

