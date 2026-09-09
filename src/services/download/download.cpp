#include <optional>

#include <pwnit/core/libcdb/libcdb.hpp>
#include <pwnit/services/deb/deb.hpp>
#include <pwnit/services/download/download.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/download.hpp>
#include <pwnit/utils/file.hpp>

namespace pwnit::download
{

constexpr auto LD = 0;
constexpr auto LIBC = 1;

constexpr std::string ld_so = "ld-linux.so";
constexpr std::string libc_so = "libc.so.6";

constexpr std::string_view launchpad_url =
    "https://launchpad.net/ubuntu/+archive/primary/+files/";

static std::pair<std::string, std::string>
copy_ld_and_libc(const std::string &path)
{
    const auto files = utils::find_files(path, {"ld-linux", "libc-"});

    if (files[LIBC])
        utils::copy_file(*files[LIBC], libc_so);

    if (files[LD]) {
        utils::copy_file(*files[LD], ld_so);
        return {ld_so, libc_so};
    }

    if (const auto file = utils::find_file(path, "ld-")) {
        utils::copy_file(*file, ld_so);
        return {ld_so, libc_so};
    }

    console::error("Couldn't find ld in {}", path);
    return {};
}

static std::string
find_launchpad_url(const libc::Libc &libc)
{
    const auto url = libcdb::find(libc);
    const auto pos = url.find_last_of('/');

    return std::string {launchpad_url} + url.substr(pos);
}

static std::string
get_unstripped_libc_url(std::string &url)
{
    const auto pos = url.rfind("/libc6_");

    if (pos != std::string::npos)
        url.replace(pos, 7, "/libc6-dbg_");

    return url;
}

static std::pair<std::string, std::string>
download_ld_and_libc(const std::string &url)
{
    console::success("Downloading libc: {}", url);

    if (!utils::wget(url))
        return {};

    const auto &path = deb::extract(utils::pkg);
    return copy_ld_and_libc(path);
}

static std::optional<std::string>
find_debug_symbols(
    const std::string &build_id, const std::string &path
) {
    if (build_id.size() < 3)
        return std::nullopt;

    const auto symbols =
        std::filesystem::path(path) / "usr/lib/debug/.build-id" /
        build_id.substr(0, 2) / (build_id.substr(2) + ".debug");

    if (!std::filesystem::is_regular_file(symbols)) {
        console::error("Couldn't find debug symbols in {}", symbols.string());
        return std::nullopt;
    }

    return symbols.string();
}

static std::optional<std::string>
download_debug_symbols(
    const std::string &build_id, std::string &url
) {
    url = get_unstripped_libc_url(url);

    console::success("Downloading libc debug symbols: {}", url);

    if (!utils::wget(url))
        return std::nullopt;

    const auto &path = deb::extract(utils::pkg);
    return find_debug_symbols(build_id, path);
}

bool unstrip_libc(
    const libc::Libc &libc, std::string &url
) {
    const auto symbols =
        download_debug_symbols(libc.build_id, url);

    if (!symbols) return false;
    return libc.unstrip(*symbols);
}

bool unstrip_libc(const libc::Libc &libc)
{
    auto url = find_launchpad_url(libc);
    return unstrip_libc(libc, url);
}

std::pair<std::string, std::string>
download(const libc::Libc &libc)
{
    std::string url = find_launchpad_url(libc);

    const auto [ld, libc_path] =
        download_ld_and_libc(url);

    if (ld.empty() || libc_path.empty())
        return {};

    if (!unstrip_libc(
            libc::Libc {libc_path}, url))
        return {ld, ""};

    return {ld, libc_path};
}

}
