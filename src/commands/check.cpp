#include <pwnit/commands.hpp>
#include <pwnit/check/check.hpp>

namespace pwnit::commands
{    
    CheckCommand::CheckCommand(CLI::App &app)
    {
        auto cmd = app.add_subcommand(
            "check", "Checks elf properties"
        );

        cmd->add_option("FILE", this->opt.file, "elf file")
            ->required();

        cmd->add_flag("--symbols", this->opt.symbols, "load symbols");
        cmd->add_flag("--sections", this->opt.sections, "load sections");
        
        cmd->callback([&] () {
            pwnit::checksec::checksec(this->opt);
        });
    }
};
