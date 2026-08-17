#include <pwnit/commands.hpp>
#include <pwnit/console/console.hpp>
#include <pwnit/elf/elf.hpp>
#include <pwnit/start/start.hpp>

namespace pwnit::start
{

enum class FileType
{
    NOT_ELF,
    ELF,
    LIBC,
    LOADER,
};

static FileType classify(const std::filesystem::path &filepath)
{
    elf::Elf e {filepath, elf::DONT_DIE_ON_ERR};
    
    if (!e.valid)
        return FileType::NOT_ELF;
    
    if (e.is_libc())
        return FileType::LIBC;
    
    if (e.is_loader())
        return FileType::LOADER;
    
    return FileType::ELF;
}

void find_binaries(commands::StartOptions &opt)
{
    for (const auto& entry :
             std::filesystem::directory_iterator(std::filesystem::current_path())) {
        
        if (entry.is_regular_file()) {
            switch (classify(entry.path())) {
            case FileType::ELF:
                if (opt.elf.empty())
                    opt.elf = entry.path();
                break;
                
            case FileType::LIBC:
                if (opt.libc.empty())
                    opt.libc = entry.path();
                break;
                
            case FileType::LOADER:
                if (opt.ld.empty())
                    opt.ld = entry.path();
                break;
                
            default: ;
            }
        }

        if (!opt.elf.empty() && !opt.ld.empty() && !opt.libc.empty())
            return;
    }
}

void print_binaries(const commands::StartOptions &opt)
{
    if (opt.elf.empty())
        console::error("Elf not found");
    else
        console::success("Elf: {}", opt.elf);

    if (opt.libc.empty())
        console::error("Libc not found");
    else
        console::success("Libc: {}", opt.libc);

    if (opt.ld.empty())
        console::error("Loader not found");
    else
        console::success("Loader: {}", opt.ld);
}
    
}
