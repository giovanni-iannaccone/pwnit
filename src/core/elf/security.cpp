#include <pwnit/core/elf/elf.hpp>

#include <LIEF/ELF.hpp>

namespace pwnit::elf
{

static
bool has_bind_now(const LIEF::ELF::Binary &binary)
{
    using TAG = LIEF::ELF::DynamicEntry::TAG;

    if (binary[TAG::BIND_NOW])
        return true;

    if (const auto* entry = binary[TAG::FLAGS]) {
        if (const auto* flags =
            entry->cast<LIEF::ELF::DynamicEntryFlags>()) {

            if (flags->has(
                LIEF::ELF::DynamicEntryFlags::FLAG::BIND_NOW))
                return true;
        }
    }

    if (const auto* entry = binary[TAG::FLAGS_1]) {
        if (const auto* flags =
            entry->cast<LIEF::ELF::DynamicEntryFlags>()) {

            if (flags->has(
                LIEF::ELF::DynamicEntryFlags::FLAG::NOW))
                return true;
        }
    }

    return false;
}

static
RELRO has_relro(const LIEF::ELF::Binary &binary)
{
    if (!binary.has(LIEF::ELF::Segment::TYPE::GNU_RELRO))
        return RELRO::NONE;

    if (has_bind_now(binary))
        return RELRO::FULL;

    return RELRO::PARTIAL;
}

enum Mask {
    CANARY = 0,
    NX = 1,
    PIE = 2,
    RELRO_ = 3,
    STATICAL = 5,
    STRIPPED = 6
};

SecurityMeasures::SecurityMeasures(const LIEF::ELF::Binary &binary)
    : metadata(0)
{
    this->set_canary(
        binary.has_dynamic_symbol("__stack_chk_fail")
    );

    this->set_nx(
        binary.has_nx()
    );

    this->set_pie(
        binary.is_pie()
    );

    this->set_relro(
        has_relro(binary)
    );

    this->set_statical(
        binary.interpreter().empty()
    );

    this->set_stripped(
        !binary.has_section(".symtab")
    );
}

uint8_t SecurityMeasures::canary() const noexcept
{
    return (metadata >> Mask::CANARY) & 1u;
}

uint8_t SecurityMeasures::nx() const noexcept
{
    return (metadata >> Mask::NX) & 1u;
}

uint8_t SecurityMeasures::pie() const noexcept
{
    return (metadata >> Mask::PIE) & 1u;
}

uint8_t SecurityMeasures::relro() const noexcept
{
    return (metadata >> Mask::RELRO_) & 0b11u;
}

uint8_t SecurityMeasures::statical() const noexcept
{
    return (metadata >> Mask::STATICAL) & 1u;
}

uint8_t SecurityMeasures::stripped() const noexcept
{
    return (metadata >> Mask::STRIPPED) & 1u;
}

void SecurityMeasures::set_canary(uint8_t value) noexcept
{
    if (value)
        metadata |= 1u << Mask::CANARY;
    else
        metadata &= ~(1u << Mask::CANARY);
}

void SecurityMeasures::set_nx(uint8_t value) noexcept
{
    if (value)
        metadata |= 1u << Mask::NX;
    else
        metadata &= ~(1u << Mask::NX);
}

void SecurityMeasures::set_pie(uint8_t value) noexcept
{
    if (value)
        metadata |= 1u << Mask::PIE;
    else
        metadata &= ~(1u << Mask::PIE);
}

void SecurityMeasures::set_relro(uint8_t value) noexcept
{
    constexpr uint8_t mask = 0b11u << Mask::RELRO_;

    metadata &= ~mask;
    metadata |= (value & 0b11u) << Mask::RELRO_;
}

void SecurityMeasures::set_statical(uint8_t value) noexcept
{
    if (value)
        metadata |= 1u << Mask::STATICAL;
    else
        metadata &= ~(1u << Mask::STATICAL);
}

void SecurityMeasures::set_stripped(uint8_t value) noexcept
{
    if (value)
        metadata |= 1u << Mask::STRIPPED;
    else
        metadata &= ~(1u << Mask::STRIPPED);
}

}
