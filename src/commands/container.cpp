#include <pwnit/commands.hpp>
#include <pwnit/container/container.hpp>

#include <CLI/CLI.hpp>

namespace pwnit::commands
{    
    ContainerCommand::ContainerCommand(CLI::App &app)
    {
        auto cmd = app.add_subcommand(
            "container", "Extracts libc and ld from Docker/Podman container"
        );
        
        cmd->add_option("id", this->opt.container_id, "Container ID")
            ->required();

        cmd->add_option("port", this->opt.port, "Process port")
            ->required();

        auto *docker = cmd->add_flag("--docker", "Use Docker");
        auto *podman = cmd->add_flag("--podman", "Use Podman");
        
        docker->excludes(podman);
        podman->excludes(docker);
        
        cmd->callback([&] () {
            this->opt.type = (docker->count())
                ? ContainerType::DOCKER
                : ContainerType::PODMAN;

            container::extract(this->opt);
        });
    }
};
