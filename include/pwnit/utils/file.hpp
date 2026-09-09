#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <ranges>
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

static inline
std::optional<std::filesystem::path>
find_in_filesystem(
    const std::filesystem::path &root, std::string_view startswith, CheckFunction func
) {
    for (const auto& entry: std::filesystem::recursive_directory_iterator(root)) {
        if (func(entry)) continue;

        const auto &name = entry.path().filename().string();
        
        if (name.starts_with(startswith))
            return entry.path();
    }

    return std::nullopt;
}
    
inline
std::optional<std::filesystem::path>
find_file(
    const std::filesystem::path& root, std::string_view startswith
) {
    CheckFunction check_func = [] (const std::filesystem::directory_entry &entry) {
        return !entry.is_regular_file() || entry.is_symlink();
    };

    return find_in_filesystem(root, startswith, check_func);
}

template <typename T>
static inline bool
filled(const std::vector<std::optional<T>> &vec) noexcept
{
    return !vec.empty() && std::ranges::all_of(vec, [](const auto& x) {
        return x.has_value();
    });
}

using Files = std::vector<std::optional<std::filesystem::path>>;
    
inline Files find_files(
    const std::filesystem::path &root, const std::vector<std::string_view> startswith
) {
    Files result;
    result.resize(startswith.size());
    
    for (const auto& entry: std::filesystem::recursive_directory_iterator(root)) {
        if (!entry.is_regular_file() || entry.is_symlink())
            continue;

        const auto &name = entry.path().filename().string();

        for (const auto &[i, prefix]: std::views::enumerate(startswith))
            if (name.starts_with(prefix))
                result[static_cast<size_t>(i)] = entry.path();

        if (filled(result))
            return result;
    }

    return result;
}

inline
std::optional<std::filesystem::path>
find_folder(
    const std::filesystem::path& root, std::string_view startswith
) {
    CheckFunction check_func = [] (const std::filesystem::directory_entry &entry) {
        return !entry.is_directory();
    };

    return find_in_filesystem(root, startswith, check_func);
}
    
inline
void give_exec_permission(const std::string &elf)
{
    if (!std::filesystem::exists(elf))
        return;
    
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
