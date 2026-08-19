#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

#include <pwnit/commands.hpp>
#include <pwnit/services/patch/patch.hpp>
#include <pwnit/services/system/system.hpp>
#include <pwnit/utils/assert.hpp>

namespace pwnit::patch
{

static
void set_interpreter(const std::string &output, const std::string &ld)
{
    if (!ld.empty()) {
        auto result = system::run(
        	std::format("patchelf --set-interpreter {} {}", ld, output)
    	);

        assert::fail(result && *result == 0, "Failed to run patchelf");
    }
}
    
static
void set_rpath(const std::string &output, const std::string &libc_dir)
{
    if (!libc_dir.empty()) {
        auto result = system::run(
            std::format("patchelf --set-rpath {} {}", libc_dir, output)
        );

        assert::fail(result && *result == 0, "Failed to run patchelf");
    }

}
    
std::string patchelf(const commands::StartOptions &opt)
{
    const auto output = opt.elf + "_patched";

    std::filesystem::copy_file(
        opt.elf,
        output,
        std::filesystem::copy_options::overwrite_existing
    );

    const std::string libc_dir = std::filesystem::path(opt.libc).parent_path();
    const std::string ld = std::filesystem::path(opt.ld);
    
    set_rpath(output, libc_dir);
    set_interpreter(output, ld);
    
    return output;
}

}

