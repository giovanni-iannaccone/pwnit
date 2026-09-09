#include <pwnit/core/libc/libc.hpp>
#include <pwnit/services/download/download.hpp>
#include <pwnit/utils/console.hpp>

#include <libc/libc.hpp>

namespace pwnit::libc
{

void info(const commands::LibcOptions &opt)
{
    libc::Libc lib {opt.file};
    lib.print_debug_info();
}
    
void unstrip(const commands::LibcOptions &opt)
{
    libc::Libc lib {opt.file};
    
    if (download::unstrip_libc(lib))
        console::success("Successfully unstripped libc");
    else
        console::error("Couldn't unstrip libc");
}
    
}
