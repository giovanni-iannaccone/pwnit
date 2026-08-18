#pragma once

#include <bit7z/bit7z.hpp>
#include <bit7z/bitformat.hpp>

namespace pwnit::ar {

constexpr auto default_path = "/tmp/pwnit_extr_path";

inline
void extract(const std::string& path)
{
    bit7z::Bit7zLibrary lib {path};

    bit7z::BitFileExtractor extractor {
        lib,
        bit7z::BitFormat::Deb
    };

    extractor.extract(
        path,
        default_path
    );
}

}
