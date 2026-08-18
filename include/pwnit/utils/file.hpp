#pragma once

#include <fstream>

namespace pwnit::utils
{
    inline std::string read_whole_file(std::ifstream &file)
    {
        return {
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        };
    }
}
