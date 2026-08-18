#include <filesystem>

#include <pwnit/commands.hpp>
#include <pwnit/download/download.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/core/libc/libc.hpp>
#include <pwnit/start/start.hpp>
#include <pwnit/utils/assert.hpp>

namespace pwnit::start
{

static inline
void give_exec_permission(const std::string &elf)
{
    std::filesystem::permissions(
        elf,
        std::filesystem::perms::owner_exec,
        std::filesystem::perm_options::add
    );
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
    
    if (need_different_loader(opt)) {
        const auto lib = libc::identify(opt.libc);
        lib.print_debug_info();
        opt.ld = download::from_libc_db(lib);
    }

    opt.elf = patchelf(opt);

    give_exec_permission(opt.elf);
    write_solve(opt);
}
    
};
