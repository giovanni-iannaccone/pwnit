#include <pwnit/commands.hpp>
#include <start/start.hpp>

namespace pwnit::commands
{    
    StartCommand::StartCommand(CLI::App &app)
    {
        auto cmd = app.add_subcommand(
            "start", "Downloads loader, downloader and unstrip loader and write solve.py"
        );

        cmd->add_option("FILE", this->opt.elf, "elf file");
        cmd->add_option("LIBC", this->opt.libc, "libc used by the elf");
        cmd->add_option("LD", this->opt.ld, "loader");
        
        cmd->callback([&] () {
            start::start(this->opt);
        });
    }
};
