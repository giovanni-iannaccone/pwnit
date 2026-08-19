#include <cstdlib>
#include <optional>
#include <string>

#include <sys/wait.h>
#include <unistd.h>

#include <pwnit/services/system/system.hpp>

namespace pwnit::system
{

std::optional<int> run(const std::string &cmd)
{
    const int status = std::system(cmd.c_str());

    if (status == -1)
        return std::nullopt;

    if (!WIFEXITED(status))
        return std::nullopt;

    return WEXITSTATUS(status);
}

}
