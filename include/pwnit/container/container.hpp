#pragma once

#include <format>
#include <string>

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
    
    httplib::Result exec(const std::vector<std::string> &cmd);    
    void getfile(const std::string &file);
    
    void set_address_family(int fam);
};

int extract(const commands::ContainerOptions &opt);
    
}
