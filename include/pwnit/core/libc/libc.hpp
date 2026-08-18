#pragma once

#include <string>

namespace pwnit::libc
{

struct Libc
{
    std::string build_id;
    std::string soname;
    std::string version;

    void print_debug_info() const noexcept;
};

Libc identify(const std::string &path);
}
