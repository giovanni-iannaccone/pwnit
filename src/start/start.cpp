#include <pwnit/commands.hpp>
#include <pwnit/download/download.hpp>
#include <pwnit/elf/elf.hpp>
#include <pwnit/start/start.hpp>

namespace pwnit::start
{

void patchelf(commands::StartOptions &_)
{

}

void start(commands::StartOptions &opt)
{
    find_binaries(opt);
    print_binaries(opt);
    
    if (opt.libc.size() && !opt.ld.size()) {
        libc::Libc libc = libc::identify(opt.libc);
        opt.ld = download::loader(libc);
    }
    
    if (opt.libc.size())
        patchelf(opt);
    
    write_solve(opt);
}
    
};
