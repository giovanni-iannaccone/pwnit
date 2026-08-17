#include <pwnit/commands.hpp>
#include <pwnit/config/config.hpp>

int main(int argc, char **argv)
{
    auto &cfg = pwnit::config::Config::instance();
    cfg.parse();
    
    CLI::App app {"pwnit - binary exploitation toolkit"};

    pwnit::commands::AnalysisCommand analysis {app};
    pwnit::commands::CheckCommand chk {app};
    pwnit::commands::StartCommand start {app};
    
    CLI11_PARSE(app, argc, argv);
}
