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

}
