#include <format>
#include <regex>

#include <pwnit/core/config/config.hpp>
#include <pwnit/core/libc/libc.hpp>
#include <pwnit/services/system/system.hpp>
#include <pwnit/utils/console.hpp>

#include <LIEF/ELF.hpp>
#include <LIEF/ELF/Binary.hpp>

namespace pwnit::libc
{

static std::string
get_build_id(const std::unique_ptr<LIEF::ELF::Binary> &binary)
{
    const LIEF::ELF::Note* note =
        binary->get(LIEF::ELF::Note::TYPE::GNU_BUILD_ID);

    if (note == nullptr)
        return "";

    std::ostringstream ss;

    for (uint8_t byte : note->description()) {
        ss << std::hex
           << std::setw(2)
           << std::setfill('0')
           << static_cast<unsigned>(byte);
    }

    return ss.str();
}

static std::string
get_version(
    const std::unique_ptr<LIEF::ELF::Binary>& binary
) {
    std::string highest;
    int highest_major = -1;
    int highest_minor = -1;

    static const std::regex glibc_regex(R"(^GLIBC_(\d+)\.(\d+))");

    for (const auto& definition : binary->symbols_version_definition()) {
        for (const auto& aux : definition.symbols_aux()) {
            const std::string& name = aux.name();

            if (name.find("PRIVATE") != std::string::npos)
                continue;

            std::smatch match;
            if (!std::regex_match(name, match, glibc_regex))
                continue;

            const int major = std::stoi(match[1].str());
            const int minor = std::stoi(match[2].str());

            if (major > highest_major ||
                (major == highest_major && minor > highest_minor)) {
                highest = name;
                highest_major = major;
                highest_minor = minor;
            }
        }
    }

    return highest;
}

Libc::Libc(const std::string &path)
{
    const auto binary = LIEF::ELF::Parser::parse(path);

    this->build_id = get_build_id(binary);
    this->soname = path;
    this->version = get_version(binary);
}

void Libc::print_debug_info() const noexcept
{
    console::log("Libc info:");
    console::log(
        "soname: {} version: {}\tbuild id: {}\t",
        std::filesystem::path(this->soname).filename().string(),
        this->version, this->build_id
    );
}
    
bool Libc::unstrip(const std::string& symbols) const
{
    const auto output = this->soname + ".unstripped";

    if (system::run(
        "eu-unstrip -o '{}' '{}' '{}'",
        output, this->soname, symbols
    ).value() != OK)
        return false;

    try {
        std::filesystem::rename(output, this->soname);
    } catch (const std::filesystem::filesystem_error &e) {
        console::error("Couldn't replace libc: {}", e.what());
        std::filesystem::remove(output);
        return false;
    }

    return true;
}
    
}
