#include <filesystem>
#include <fstream>

#include <pwnit/config/config.hpp>
#include <pwnit/download/download.hpp>
#include <pwnit/utils/console.hpp>

#include <httplib.h>
#include <nlohmann/json.hpp>

namespace pwnit::download
{

static
size_t write_string(char *ptr, size_t size, size_t nmemb, void *data)
{
    auto &out = *static_cast<std::string *>(data);
    out.append(ptr, size * nmemb);
    return size * nmemb;
}

static
size_t write_file(char *ptr, size_t size, size_t nmemb, void *data)
{
    auto &file = *static_cast<std::ofstream *>(data);
    file.write(ptr, static_cast<long>(size * nmemb));
    return size * nmemb;
}

static nlohmann::json find(const libc::Libc &lib)
{
    return {};
}

static
std::string download_file(const std::string &url)
{
    const auto path = std::filesystem::path(url).filename();
    return "";
}

std::string from_libc_db(libc::Libc lib)
{
    const auto response = find(lib);
    if (!response)
        return "";
            
    return download_file(
        response["download_url"].get<std::string>()
    );
}

}
