#include <pwnit/core/config/config.hpp>
#include <pwnit/core/libc/libc.hpp>
#include <pwnit/core/libcdb/libcdb.hpp>
#include <pwnit/utils/ar.hpp>
#include <pwnit/utils/console.hpp>

#include <httplib.h>
#include <nlohmann/json.hpp>

namespace pwnit::libcdb
{

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
httplib::Result get(std::string_view url)
{
    std::string base { url.substr(0, url.find('/', 8)) };
    auto endpoint = url.substr(base.size());

    httplib::Client client (base);
    
    return client.Get(
        std::string{endpoint}
    );
}

static
std::string save_package(const std::string &body)
{
    constexpr auto pkg = "package.deb";
    
    std::ofstream file (pkg, std::ios::binary);
    file.write(body.data(), static_cast<long>(body.size()));

    return pkg;
}
    
std::pair<std::string, std::string>
download(const libc::Libc &lib)
{
    std::string url = find(lib);
    console::success("\nTrying to download {}", url);
    auto res = get(url);
    
    if (!res || res->status != 200)
        return {};

    auto pkg = save_package(res->body);
    ar::extract(pkg);

    return {};
}
    
}
