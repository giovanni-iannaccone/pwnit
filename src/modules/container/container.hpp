#pragma once

#include <format>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include <pwnit/commands.hpp>

#include <httplib.h>

namespace pwnit::container
{

namespace docker
{
    constexpr auto sock = "/var/run/docker.sock";    

    constexpr std::string get_socket()
    {
        return sock;
    }
}

namespace podman
{
    constexpr auto sock = "/run/user/{}/podman/podman.sock";

    inline std::string get_socket()
    {
        auto uid = static_cast<unsigned long>(getuid());    
        return std::format(sock, uid);
    }
}

struct ContainerClient
{
    httplib::Client client;
    const std::string container_id;

    ContainerClient(const std::string &sock, const std::string &container_id)
        : client({std::move(sock)}),
          container_id(container_id)
    {
        client.set_address_family(AF_UNIX);
    }
    
    std::optional<httplib::Result>
    exec(const std::vector<std::string> &cmd);
    
    bool getfile(const std::string &file);
};

void extract(const commands::ContainerOptions &opt);
    
}
