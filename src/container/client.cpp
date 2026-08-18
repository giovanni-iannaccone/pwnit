#include <filesystem>
#include <fstream>
#include <optional>

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
    
    std::optional<httplib::Result>
    exec(const std::vector<std::string> &cmd)
    {
        nlohmann::json body = {
            {"AttachStdout", true},
            {"AttachStderr", true},
            {"Tty", false},
            {"Cmd", cmd}
        };

        const auto endp = std::format(prepcmd_endpoint, container_id);

        auto res = client.Post(
            endp,
            body.dump(),
            "application/json"
        );

        if (!res || res->status != 201)
            return std::nullopt;
        
        auto create_response = nlohmann::json::parse(res->body);
        std::string exec_id = create_response["Id"];
        
        body = {
            {"Detach", false},
            {"Tty", false}
        };
        
        res = client.Post(
           "/exec/" + exec_id + "/start",
           body.dump(),
           "application/json"
        );

        if (!res || res->status != 201)
            return std::nullopt;

        return res;
    }
    
    bool getfile(const std::string &file)
    {
        const auto endp = std::format(getfile_endpoint, container_id, file);
        auto res = client.Get(endp);

        const auto path = std::filesystem::path(file).filename();

        std::ofstream fd (path);

        if (!fd)
            return false;

        fd << res->body;
        return true;
    }

    void set_address_family(int fam)
    {
        return client.set_address_family(fam);
    }
};

}
