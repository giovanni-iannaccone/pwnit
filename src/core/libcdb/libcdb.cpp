#include <filesystem>

#include <pwnit/core/config/config.hpp>
#include <pwnit/core/libc/libc.hpp>
#include <pwnit/core/libcdb/libcdb.hpp>
#include <pwnit/services/deb/deb.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/file.hpp>

#include <httplib.h>
#include <nlohmann/json.hpp>

namespace pwnit::libcdb
{

constexpr std::string_view ld_so = "ld-linux.so";
constexpr std::string_view libc_so = "libc.so.6";

constexpr std::string_view launchpad_url = "https://launchpad.net/ubuntu/+archive/primary/+files/";
constexpr std::string_view pkg = "/tmp/package.deb";

static
void copy_ld(const std::string &path)
{
    const auto file = utils::find_file(path, "ld-linux");
    if (!file.has_value()) {
        console::error("Couldn't find ld in {}", path);
        return;
    }
    
    utils::copy_file(file.value(), ld_so);
}

static
void copy_libc(const libc::Libc &libc, const std::string &path)
{
    const auto folder = utils::find_folder(path, libc.build_id.substr(0, 2));
    if (!folder.has_value()) {
        console::error("Couldn't find valid libc in extracted package");
        return;
    }
    
    const auto libc_path =
        utils::find_file(
            folder.value().string(),
			libc.build_id.substr(folder.value().filename().string().size(), libc.build_id.size())
        );

    if (!libc_path.has_value()) {
        console::error("Couldn't find valid libc in extracted package");
        return;
    }
    
    utils::copy_file(libc_path.value(), libc_so);
}
    
static 
std::string find(const libc::Libc &libc)
{
    const auto &cfg = config::Config::instance();
    httplib::Client client (cfg.libc_db);
    
    nlohmann::json body = {
        {"buildid", libc.build_id}
    };
    
    auto res = client.Post(
        "/api/find",
        body.dump(),
        "application/json"
    );
    
    const auto res_body  =
        res->body.substr(1, res->body.size() - 3);

    try {
        return nlohmann::json::parse(res_body)["libs_url"];
    } catch (...) {
        return "";
    }
}

static
std::string find_launchpad_url(const libc::Libc &libc)
{
    std::string url = find(libc);
    std::size_t pos = url.find_last_of('/');

    return std::string {launchpad_url} + url.substr(pos);    
}

static
std::string get_unstripped_libc_url(std::string &url)
{
    const auto pos = url.rfind("/libc6_");

    if (pos != std::string::npos)
        url.replace(pos, 7, "/libc6-dbg_");

    return url;
}
    
static
bool save_package(const std::string &body)
{    
    std::ofstream file (std::string {pkg}, std::ios::binary);
    if (!file)
        return false;
    
    file.write(body.data(), static_cast<long>(body.size()));
    return true;
}
    
static
bool wget(std::string_view url)
{
    std::string base { url.substr(0, url.find('/', 8)) };
    auto endpoint = url.substr(base.size());

    httplib::Client client (base);
    client.set_follow_location(true);
    
    const auto res = client.Get(
        std::string {endpoint}
    );

    if (!res || res->status != 200)
        return false;
    
    return save_package(res->body);
}

static std::string
download_ld(const std::string &url)
{
    
    console::success("\nTrying to download ld from {}", url);
    if (!wget(url))
        return "";

    const auto path = deb::extract(pkg);
    copy_ld(path);

    return std::string {ld_so};
}

static std::string
download_unstripped_libc(const libc::Libc &libc, std::string &url)
{
    url = get_unstripped_libc_url(url);
    
    console::success("\nTrying to download unstripped libc from {}", url);
    if (!wget(url))    
        return libc.soname;
    
    const auto path = deb::extract(pkg);
    copy_libc(libc, path);
    
    return std::string {libc_so};
}

std::pair<std::string, std::string>
download(const libc::Libc& libc)
{
    std::string url = find_launchpad_url(libc);

    const auto ld = download_ld(url);
    const auto lib = download_unstripped_libc(libc, url);

    return {ld, lib};
}
    
}
