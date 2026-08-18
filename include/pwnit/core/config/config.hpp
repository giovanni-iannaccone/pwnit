#pragma once

#include <filesystem>

#include <toml++/toml.hpp>

namespace pwnit::config
{

static std::filesystem::path default_path =
    std::filesystem::path(std::getenv("HOME")) / ".config/pwnit/pwnit.toml";

struct Config
{

private:
    Config() = default;

public:
    std::string solve_file;
    std::string template_file;
    std::string libc_db;
    toml::array *dangerous_functions;
    
    static Config &instance() noexcept
    {
        static Config config;
        return config;
    }

    void parse();
};
    
}
