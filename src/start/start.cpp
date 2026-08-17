#include <filesystem>

#include <pwnit/commands.hpp>
#include <pwnit/download/download.hpp>
#include <pwnit/elf/elf.hpp>
#include <pwnit/start/start.hpp>
#include <pwnit/utils/utils.hpp>

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
    return opt.libc.empty() && opt.ld.empty();
}

void start(commands::StartOptions &opt)
{
    find_binaries(opt);
    print_binaries(opt);

    utils::assert_fail(
        !opt.elf.empty(),
        "Could not find target ELF"
    );
    
    if (need_different_loader(opt)) {
        const auto lib = libc::identify(opt.libc);
        opt.ld = download::from_libc_db(lib);
    }

    opt.elf = patchelf(opt);

    give_exec_permission(opt.elf);
    write_solve(opt);
}
    
};
