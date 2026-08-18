#include <pwnit/commands.hpp>
#include <pwnit/core/config/config.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/core/libc/libc.hpp>
#include <pwnit/core/libcdb/libcdb.hpp>
#include <pwnit/services/patch/patch.hpp>
#include <pwnit/utils/assert.hpp>
#include <pwnit/utils/console.hpp>
#include <pwnit/utils/file.hpp>

#include <start/start.hpp>

namespace pwnit::start
{

static inline
bool download_from_libcdb(const libc::Libc &libc)
{
    if (libcdb::download(libc).first.empty()) {
        console::error("Couldn't download libc and ld (libc.rip api often gives wrong results)");
        return false;
    }

    console::success("Successfully downloaded libc");
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

    assert::fail(
        !opt.elf.empty(),
        "Could not find target ELF"
    );
    
    if (opt.libc.empty())
        return;
    
    const auto lib = libc::identify(opt.libc);
    lib.print_debug_info();

    if (need_different_loader(opt)
        && download_from_libcdb(lib)) {
            opt.elf = patch::patchelf(opt);
            console::success("Created {}", opt.elf);
    }

    utils::give_exec_permission(opt.elf);
    write_solve(opt);
}
    
};
