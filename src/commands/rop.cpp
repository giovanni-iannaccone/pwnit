#include <pwnit/commands.hpp>
#include <rop/rop.hpp>

namespace pwnit::commands
{
    RopCommand::RopCommand(CLI::App &app)
    {
        auto cmd = app.add_subcommand(
            "rop", "Finds rop gadgets in an elf file"
        );

        cmd->add_option("FILE", this->opt.elf, "elf file")
            ->required();

        cmd->add_option("--depth", this->opt.depth, "number of instructions before ret (default 5)")
            ->check(CLI::Range(1, 255))
            ->default_val(5);
        
        cmd->add_option("--search", this->opt.search, "search for a specific gadget");
        
        cmd->add_flag("--json", this->opt.json, "print data in json format");
        
        cmd->callback([&] () {
            rop::gadgets(this->opt);
        });
    }
};
