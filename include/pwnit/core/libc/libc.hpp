#pragma once

#include <string>

namespace pwnit::libc
{

struct Libc
{
    std::string build_id;
    std::string soname;
    std::string version;

    Libc(const std::string &path);
    
    void print_debug_info() const noexcept;
    bool unstrip(const std::string &symbols) const;
};

}
