#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

#include <pwnit/commands.hpp>
#include <pwnit/start/start.hpp>
#include <pwnit/utils/assert.hpp>

namespace pwnit::start
{

static
void run_patchelf(const std::vector<std::string> &args)
{
    std::vector<char*> argv;
    argv.reserve(args.size() + 1);

    for (const auto& arg : args)
        argv.push_back(const_cast<char*>(arg.c_str()));

    argv.push_back(nullptr);

    const pid_t pid = fork();
    
    assert::fail(
        pid >= 0,
        "Could not fork for patchelf"
    );

    if (pid == 0) {
        execvp(argv[0], argv.data());
        std::exit(EXIT_FAILURE);
    } 

    int status = 0;

    assert::fail(
        waitpid(pid, &status, 0) >= 0,
        "Could not wait for patchelf"
    );

    assert::fail(
        WIFEXITED(status) && WEXITSTATUS(status) == 0,
        "patchelf failed"
    );
}
    
std::string patchelf(const commands::StartOptions &opt)
{
    const auto output = opt.elf + "_patched";

    std::filesystem::copy_file(
        opt.elf,
        output,
        std::filesystem::copy_options::overwrite_existing
    );

    const auto libc_dir = std::filesystem::path(opt.libc).parent_path();
    const auto ld = std::filesystem::path(opt.ld);

    run_patchelf({
        "patchelf",
        "--set-rpath",
        libc_dir.string(),
        output
    });

    run_patchelf({
        "patchelf",
        "--set-interpreter",
        ld.string(),
        output
        });

    return output;
}

}

