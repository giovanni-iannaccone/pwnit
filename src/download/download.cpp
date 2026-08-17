#include <filesystem>
#include <fstream>

#include <pwnit/config/config.hpp>
#include <pwnit/console/console.hpp>
#include <pwnit/download/download.hpp>

#include <curl/curl.h>
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
    CURL *curl = curl_easy_init();
    std::string response;
    
    const auto body = nlohmann::json{
        {"buildid", lib.build_id}
    }.dump();

    const auto &cfg = config::Config::instance();
    
    curl_easy_setopt(curl, CURLOPT_URL, cfg.libc_db.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    if (curl_easy_perform(curl) != CURLE_OK) {
        console::error("libc.rip request failed");
        return nullptr;
    }
    
    curl_easy_cleanup(curl);

    auto result = nlohmann::json::parse(response);

    if (result.empty()) {
        console::error("libc not found");
        return "";
    }
    
    return result[0];
}

static
std::string download_file(const std::string &url)
{
    const auto path = std::filesystem::path(url).filename();

    std::ofstream file{path, std::ios::binary};
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &file);

    if (curl_easy_perform(curl) != CURLE_OK)
        console::error("download failed");

    curl_easy_cleanup(curl);
    return path.string();
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
