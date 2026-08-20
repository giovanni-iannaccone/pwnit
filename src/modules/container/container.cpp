#include <cstdint>
#include <optional>
#include <regex>

#include <pwnit/commands.hpp>
#include <pwnit/utils/assert.hpp>

#include <container/container.hpp>

namespace pwnit::container
{

static
void download(ContainerClient &client, const std::vector<std::string> &files)
{
    for (auto &&file: files) {
        if (!client.getfile(file))
            console::error("Couldn't download file {}", std::filesystem::path(file).filename().string());
        else
            console::success("Successfully downloaded {}", file);
    }
}
    
static std::string
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

static std::pair<std::string, std::string>
find_libc_and_ld(ContainerClient &client, int pid)
{
    auto res = client.exec(
        {"cat", std::format("/proc/{}/maps", pid)}
    );
    
    assert::fail(res != std::nullopt, "Couldn't communicate with socket to check pid {}", pid);
    
    return {
        find_path(res->value().body, "libc.so"),
        find_path(res->value().body, "ld-linux")
    };
}

static
int find_process_pid(ContainerClient &client, uint16_t port)
{
    auto res = client.exec(
        {"ss", "-lp", "'sport", "=", std::format(":{}'", port)}
    );

    assert::fail(res != std::nullopt, "Couldn't communicate with socket to check service on port {}", port);

    static const std::regex re (R"(pid=(\d+))");
    std::smatch match;

    return std::regex_search(res->value().body, match, re)
        ? std::atoi(match[1].str().c_str())
        : 0;
}

static 
ContainerClient initialize_client(const commands::ContainerOptions &opt)
{
    return {
        (opt.type == commands::ContainerType::DOCKER)
        	? docker::get_socket()
        	: podman::get_socket(),
        opt.container_id
    };
}

void extract(const commands::ContainerOptions &opt)
{
    ContainerClient client = initialize_client(opt);

    int pid = find_process_pid(client, opt.port);

    const auto && [libc, ld] =
        find_libc_and_ld(client, pid);
    
    download(client, {libc, ld});
}

}
