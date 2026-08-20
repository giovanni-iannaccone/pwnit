#pragma once

#include <fstream>
#include <string>

#include <httplib.h>

namespace pwnit::utils
{

constexpr std::string_view pkg = "/tmp/package.deb";

inline
bool save_package(const std::string &body)
{    
    std::ofstream file (std::string {pkg}, std::ios::binary);
    if (!file)
        return false;
    
    file.write(body.data(), static_cast<long>(body.size()));
    return true;
}
    
inline
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

}
