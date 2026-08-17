#include <pwnit/commands.hpp>

namespace pwnit::commands
{
    AnalysisCommand::AnalysisCommand(CLI::App &app)
    {
        auto cmd = app.add_subcommand(
            "analysis", "Analyze elf to find calls to vulnerable functions"
        );
        
        cmd->add_option("FILE", this->opt.file, "elf file")
            ->required();

        cmd->callback([&] () {});
    }
};
