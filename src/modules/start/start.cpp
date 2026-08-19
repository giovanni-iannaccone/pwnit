#include <pwnit/commands.hpp>
#include <pwnit/core/config/config.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/core/libc/libc.hpp>
#include <pwnit/core/libcdb/libcdb.hpp>
#include <pwnit/services/patch/patch.hpp>
#include <pwnit/services/template/template.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/file.hpp>

#include <start/start.hpp>

namespace pwnit::start
{

static inline
bool download_from_libcdb(const libc::Libc &libc, commands::StartOptions &opt)
{
    const auto [ld_path, libc_path] = libcdb::download(libc);
    
    if (ld_path.empty()) {
        console::error("Couldn't download libc and ld (launchpad is often down lol)");
        return false;
    }

    opt.ld = ld_path;
    opt.libc = libc_path;
    return true;
}

static inline
bool need_different_loader(commands::StartOptions &opt)
{
    return !opt.libc.empty() && opt.ld.empty();
}

void start(commands::StartOptions &opt)
{
    find_binaries(opt);
    print_binaries(opt);

    assert::fail(!opt.elf.empty(), "Could not find target ELF");
    
    if (opt.libc.empty())
        return;
    
    const auto lib = libc::identify(opt.libc);
    lib.print_debug_info();

    if (need_different_loader(opt)
        && download_from_libcdb(lib, opt)) {

        opt.elf = patch::patchelf(opt);
        console::success("Created {}", opt.elf);
    }

    utils::give_exec_permission(opt.elf);
    utils::give_exec_permission(opt.ld);

    templates::write_solve(opt);
}
    
};
