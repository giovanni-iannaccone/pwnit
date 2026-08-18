#include <pwnit/commands.hpp>
#include <pwnit/config/config.hpp>

using namespace pwnit;

int main(int argc, char **argv)
{
    auto &cfg = config::Config::instance();
    cfg.parse();
    
    CLI::App app {"pwnit - binary exploitation toolkit"};

    commands::AnalysisCommand analysis {app};
    commands::CheckCommand chk {app};
    commands::ContainerCommand container {app};
    commands::StartCommand start {app};
    
    CLI11_PARSE(app, argc, argv);
}
