#pragma once

#include <string>

namespace pwnit::libc
{

constexpr auto DOWNLOAD_FAILED = 0;

struct Libc
{
    std::string build_id;
    std::string soname;
    std::string version;
};

Libc identify(const std::string &path);
}
