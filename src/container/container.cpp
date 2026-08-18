#include <cstdint>
#include <regex>

#include <pwnit/commands.hpp>
#include <pwnit/container/container.hpp>

namespace pwnit::container
{

std::string
find_path(const std::string& block, const std::string& name)
{
    std::istringstream stream (block);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.find(name) == std::string::npos)
            continue;

        std::istringstream lineStream(line);

        std::string field;
        std::string lastField;

        while (lineStream >> field)
            lastField = field;

        if (lastField.find(name) != std::string::npos)
            return lastField;
    }

    return "";
}

std::pair<std::string, std::string>
find_libc_and_ld(ContainerClient &client, int pid)
{
    auto res = client.exec(
        {"cat", std::format("/proc/{}/maps", pid)}
    );
    
    return {
        find_path(res->body, "libc.so"),
        find_path(res->body, "ld-linux")
    };
}
    
int find_process_pid(ContainerClient &client, uint16_t port)
{
    auto res = client.exec(
        {"ss", "-lp", "'sport", "=", std::format(":{}'", port)}
    );

    static const std::regex re (R"(pid=(\d+))");
    std::smatch match;

    return std::regex_search(res->body, match, re)
        ? std::atoi(match[1].str().c_str())
        : 0;
}

ContainerClient initialize_client(const commands::ContainerOptions &opt)
{
    std::string sock = (opt.type == commands::ContainerType::DOCKER)
        ? docker::get_socket()
        : podman::get_socket();

    ContainerClient client {httplib::Client (sock), opt.container_id};
    client.set_address_family(AF_UNIX);

    return client;
}

int extract(const commands::ContainerOptions &opt)
{
    ContainerClient client = initialize_client(opt);

    int pid = find_process_pid(client, opt.port);
    const auto && [libc, ld] =
        find_libc_and_ld(client, pid);
    
    client.getfile(libc);
    client.getfile(ld);

    return 0;
}

}
