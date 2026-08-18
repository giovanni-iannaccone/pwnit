#pragma once

#include <pwnit/core/libc/libc.hpp>

#include <httplib.h>
#include <nlohmann/json.hpp>

namespace pwnit::libcdb
{
    std::pair<std::string, std::string>
    download(const libc::Libc &lib);
}
