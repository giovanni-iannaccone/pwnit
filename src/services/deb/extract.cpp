#include <array>
#include <filesystem>
#include <iostream>
#include <string>

#include <pwnit/services/system/system.hpp>
#include <pwnit/utils/console.hpp>

#include <bit7z/bit7z.hpp>
#include <bit7z/bitfileextractor.hpp>
#include <bit7z/bitformat.hpp>
#include <string_view>

namespace pwnit::deb
{

constexpr std::string_view default_path = "/tmp/pwnit_extract_path";
    
constexpr std::array paths = {
    "/usr/lib/7zip/7z.so",
    "/usr/lib/p7zip/7z.so",
    "/usr/lib/x86_64-linux-gnu/7zip/7z.so",
    "/usr/lib/aarch64-linux-gnu/7zip/7z.so",
};
    
static inline
void create_extraction_path()
{   
    if (std::filesystem::exists(default_path))
        std::filesystem::remove_all(default_path);
    
    std::filesystem::create_directory(default_path);
}
    
static
const std::string &find_7z()
{
    static std::string path_7z;

    if (!path_7z.empty())
        return path_7z;

    for (const auto path : paths) {
        if (std::filesystem::is_regular_file(path)) {
            path_7z = path;
            return path_7z;
        }
    }

    console::error(
        "Couldn't find 7z library, type the path (e.g. {}): ", paths[0]
    );

    std::cin >> path_7z;
    return path_7z;
}

static
std::string find_data_filename()
{
    for (const auto &path: std::filesystem::directory_iterator(default_path))
        if (std::filesystem::is_regular_file(path) &&
            path.path().filename().string().starts_with("data.tar."))
            return path.path();

    return "";
}

static
bool extract_data_tar()
{
    const auto data_filename = find_data_filename();

    if (data_filename.empty()) {
        console::error("Couldn't find data.tar.* in '{}'", default_path);
        return false;
    }

    const auto result = system::run(
		"tar -xf {} -C {} ", data_filename, std::string {default_path}
    );

    if (!result) {
        console::error("Failed to execute tar");
        return false;
    }

    if (*result != 0) {
        console::error("tar failed with exit code {}", *result);
        return false;
    }

    return true;
}

static bool
extract_deb(std::string_view path)
{
    try {
        bit7z::Bit7zLibrary lib {find_7z()};

        bit7z::BitFileExtractor extractor {
            lib, bit7z::BitFormat::Deb
        };

        extractor.extract(
            std::string {path},
            std::string {default_path}
        );

        return true;

    } catch (const std::exception& e) {
        console::error("Internal .deb extraction failed: {}", e.what());

    } catch (...) {
        console::error("Internal .deb extraction failed with an unknown error");
    }

    return false;
}

static bool
try_dpkg_extract(std::string_view deb)
{
    const auto result = system::run(
        "dpkg --extract {} {}", std::string {deb}, std::string {default_path}
    );

    return result && *result == 0;
}

std::string
extract(std::string_view path)
{
    create_extraction_path();
    
    if (!std::filesystem::exists(path)) {
        console::error("Debian package '{}' doesn't exist", path);
        return {};
    }

    if (try_dpkg_extract(path))
        return std::string {default_path};

    console::warn("dpkg unavailable or failed, using internal extractor");

    if (!extract_deb(path)) {
        console::error("Couldn't extract Debian archive");
        return {};
    }

    if (!extract_data_tar()) {
        console::error("Couldn't extract data.tar.* from Debian archive");
        return {};
    }

    console::success("Extracted using internal extractor");
    return std::string {default_path};
}

}
