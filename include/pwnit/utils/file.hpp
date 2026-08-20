#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <string_view>

namespace pwnit::utils
{

using CheckFunction = std::function<bool (const std::filesystem::directory_entry &)>;

static inline
void copy_file(const std::string &src, std::string_view dst)
{
    std::filesystem::copy_file(
        src, dst, std::filesystem::copy_options::overwrite_existing
    );
}

static inline std::optional<std::filesystem::path>
find_in_filesystem(const std::filesystem::path &root, std::string_view startswith, CheckFunction func)
{
    for (const auto& entry: std::filesystem::recursive_directory_iterator(root)) {
        if (func(entry)) continue;

        const auto &name = entry.path().filename().string();
        
        if (name.starts_with(startswith))
            return entry.path();
    }

    return std::nullopt;

}

inline std::optional<std::filesystem::path>
find_file(const std::filesystem::path& root, std::string_view startswith)
{
    CheckFunction check_func = [] (const std::filesystem::directory_entry &entry) {
        return !entry.is_regular_file() || entry.is_symlink();
    };

    return find_in_filesystem(root, startswith, check_func);
}

inline std::optional<std::filesystem::path>
find_folder(const std::filesystem::path& root, std::string_view startswith)
{
    CheckFunction check_func = [] (const std::filesystem::directory_entry &entry) {
        return !entry.is_directory();
    };

    return find_in_filesystem(root, startswith, check_func);
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

inline std::string read_whole_file(std::ifstream &file)
{
    return {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}

}
