#pragma once

#include <filesystem>
#include <fstream>

namespace pwnit::utils
{

inline std::string read_whole_file(std::ifstream &file)
{
    return {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}

inline
void give_exec_permission(const std::string &elf)
{
    std::filesystem::permissions(
        elf,
        std::filesystem::perms::owner_exec,
        std::filesystem::perm_options::add
    );
}

}
