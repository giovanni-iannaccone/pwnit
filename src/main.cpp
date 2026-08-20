#include <pwnit/commands.hpp>
#include <pwnit/core/config/config.hpp>

using namespace pwnit::commands;
using namespace pwnit::config;

int main(int argc, char **argv)
{
    auto &cfg = Config::instance();
    cfg.parse();
    
    CLI::App app {"pwnit - binary exploitation toolkit"};

    AnalysisCommand analysis {app};
    CheckCommand chk {app};
    ContainerCommand container {app};
    RopCommand rop {app};
    StartCommand start {app};
    
    CLI11_PARSE(app, argc, argv);
}
