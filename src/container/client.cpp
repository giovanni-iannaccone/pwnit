#include <filesystem>
#include <fstream>

#include <pwnit/utils/console.hpp>

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <sys/socket.h>

namespace pwnit::container
{

constexpr auto getfile_endpoint = "/containers/{}/archive?path={}";
constexpr auto prepcmd_endpoint = "/containers/{}/exec";
    
struct ContainerClient
{
    httplib::Client client;
    const std::string container_id;
    
    httplib::Result exec(const std::vector<std::string> &cmd)
    {
        nlohmann::json body = {
            {"AttachStdout", true},
            {"AttachStderr", true},
            {"Tty", false},
            {"Cmd", cmd}
        };

        const auto endp = std::format(prepcmd_endpoint, container_id);

        return client.Post(
            endp,
            body.dump(),
            "application/json"
        );
    }
    
    void getfile(const std::string &file)
    {
        const auto endp = std::format(getfile_endpoint, container_id, file);
        auto res = client.Get(endp);

        const auto path = std::filesystem::path(file).filename();

        std::ofstream fd (path);

        if (!fd) {
            console::error("Couldn't create file {}", path);
            return;
        }

        fd << res->body;
    }

    void set_address_family(int fam)
    {
        return client.set_address_family(fam);
    }
};

}
