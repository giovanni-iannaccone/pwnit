#include <optional>

#include <pwnit/core/libcdb/libcdb.hpp>
#include <pwnit/services/deb/deb.hpp>
#include <pwnit/services/download/download.hpp>
#include <pwnit/services/system/system.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/download.hpp>
#include <pwnit/utils/file.hpp>

namespace pwnit::download
{

constexpr std::string_view ld_so = "ld-linux.so";
constexpr std::string_view launchpad_url = "https://launchpad.net/ubuntu/+archive/primary/+files/";

static void
copy_ld(const std::string &path)
{
    auto file = utils::find_file(path, "ld-linux");
    if (file.has_value()) [[likely]]
        goto copy_file;

    file = utils::find_file(path, "ld-");
    if (file.has_value()) [[likely]]
        goto copy_file;
    
    console::error("Couldn't find ld in {}", path);
    return;
    
 copy_file:
    utils::copy_file(file.value(), ld_so);
    return;
}

static std::optional<std::string>
find_debug_symbols(const std::string &build_id, const std::string &path)
{
    const auto folder = utils::find_folder(path, build_id.substr(0, 2));
    if (!assert::check(folder.has_value(),
                       "Couldn't find valid libc in extracted package"))
        return std::nullopt;
    
    const auto libc_path =
        utils::find_file(
            folder.value().string(),
			build_id.substr(folder.value().filename().string().size(), build_id.size())
        );

    if (!assert::check(libc_path.has_value(),
                       "Couldn't find valid libc in extracted package"))
        return std::nullopt;

    return libc_path.value();
}

static std::string
find_launchpad_url(const libc::Libc &libc)
{
    std::string url = libcdb::find(libc);
    std::size_t pos = url.find_last_of('/');

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

static std::string
download_ld(const std::string &url)
{
    console::success("\nDownloading ld: {}", url);
    if (!utils::wget(url))
        return "";

    const auto &path = deb::extract(utils::pkg);
    copy_ld(path);

    return std::string {ld_so};
}

static std::optional<std::string>
download_debug_symbols(const std::string &build_id, std::string &url)
{
    url = get_unstripped_libc_url(url);
    
    console::success("Unstripping libc: {}", url);
    if (!utils::wget(url))
        return std::nullopt;
    
    const auto &root = deb::extract(utils::pkg);
    return find_debug_symbols(build_id, root);
}

bool unstrip_libc(const libc::Libc &libc, std::string &url)
{    
    const auto symbols = download_debug_symbols(libc.build_id, url);
    if (!symbols.has_value())
        return false;

    return libc.unstrip(symbols.value());
}

bool unstrip_libc(const libc::Libc &libc)
{
    auto url = find_launchpad_url(libc);
    return unstrip_libc(libc, url);
}

std::pair<std::string, std::string>
download(const libc::Libc& libc)
{
    std::string url = find_launchpad_url(libc);
    const auto ld = download_ld(url);
    
    if (unstrip_libc(libc, url))
        return {ld, libc.soname};
    
    return {ld, ""};
}

}
