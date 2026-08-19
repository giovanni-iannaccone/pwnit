#include <pwnit/commands.hpp>
#include <pwnit/core/elf/elf.hpp>
#include <pwnit/utils/console.hpp>

#include <start/start.hpp>

namespace pwnit::start
{

enum class FileType
{
    NOT_ELF,
    ELF,
    LIBC,
    LOADER,
};

static FileType classify(const std::filesystem::path& filepath)
{
    elf::Elf e {filepath, elf::DONT_DIE_ON_ERR};

    if (!e.valid)
        return FileType::NOT_ELF;

    const auto filename = filepath.filename().string();

    if (filename.find("libc") != std::string::npos)
        return FileType::LIBC;

    if (filename.find("ld-") != std::string::npos)
        return FileType::LOADER;

    return FileType::ELF;
}

void find_binaries(commands::StartOptions& opt)
{
    for (const auto& entry :
         std::filesystem::directory_iterator(
             std::filesystem::current_path())) {

        if (!entry.is_regular_file())
            continue;

        const auto path = entry.path();

        switch (classify(path)) {
        case FileType::ELF:
            if (opt.elf.empty())
                opt.elf = path;
            break;

        case FileType::LIBC:
            if (opt.libc.empty())
                opt.libc = path;
            break;

        case FileType::LOADER:
            if (opt.ld.empty())
                opt.ld = path;
            break;
            
        case FileType::NOT_ELF:
            break;
        }

        if (!opt.elf.empty() && !opt.libc.empty() && !opt.ld.empty())
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

    console::log("");
}
    
}
