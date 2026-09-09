#include <pwnit/commands.hpp>
#include <libc/libc.hpp>

namespace pwnit::commands
{
    LibcCommand::LibcCommand(CLI::App &app)
    {
        auto cmd = app.add_subcommand(
            "libc", "Finds rop gadgets in an elf file"
        );

        auto unstrip = cmd->add_subcommand("unstrip", "Unstrip a libc");
        
        unstrip->callback([&] () {
            libc::unstrip(this->opt);
        });

        unstrip->add_option("FILE", this->opt.file, "libc")
            ->required();

        auto info = cmd->add_subcommand("info", "Informations about libc");
        
        info->callback([&] () {
            libc::info(this->opt);
        });

        info->add_option("FILE", this->opt.file, "libc")
            ->required();
    }
};
