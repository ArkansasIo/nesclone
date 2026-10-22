#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace nex8::hardware {

struct Profile final {
    static constexpr std::string_view name = "CELL8 Rev-A";
    static constexpr std::uint32_t targetClockHz = 8'000'000;
    static constexpr std::uint16_t addressSpaceBytes = 65'536;
    static constexpr std::size_t systemRamBytes = 8 * 1024;
    static constexpr std::size_t videoRamBytes = 2 * 1024;
    static constexpr std::size_t paletteRamBytes = 256;
    static constexpr std::size_t spriteRamBytes = 256;
    static constexpr std::size_t spuCount = 4;
    static constexpr std::size_t spuLocalRamBytes = 256;
    static constexpr std::size_t cartridgeBankBytes = 32 * 1024;
    static constexpr std::uint16_t screenWidth = 256;
    static constexpr std::uint16_t screenHeight = 240;
    static constexpr std::uint16_t refreshHz = 60;
    static constexpr std::uint16_t paletteEntries = 256;
};

enum class AddressRegion {
    systemRam, videoRam, paletteRam, spriteRam, gpuRegisters,
    audioRegisters, controllerRegisters, dmaRegisters, spuRegisters,
    cartridgeRam, cartridgeRom, unmapped
};

struct MemoryRegion final {
    std::uint16_t begin;
    std::uint16_t end;
    AddressRegion region;
    bool writable;
};

inline constexpr MemoryRegion memoryMap[] = {
    {0x0000, 0x1FFF, AddressRegion::systemRam, true},
    {0x2000, 0x27FF, AddressRegion::videoRam, true},
    {0x2800, 0x28FF, AddressRegion::paletteRam, true},
    {0x2900, 0x29FF, AddressRegion::spriteRam, true},
    {0x2A00, 0x2FFF, AddressRegion::gpuRegisters, true},
    {0x3000, 0x30FF, AddressRegion::audioRegisters, true},
    {0x3100, 0x31FF, AddressRegion::controllerRegisters, true},
    {0x3200, 0x32FF, AddressRegion::dmaRegisters, true},
    {0x3300, 0x33FF, AddressRegion::spuRegisters, true},
    {0x4000, 0x7FFF, AddressRegion::cartridgeRam, true},
    {0x8000, 0xFFFF, AddressRegion::cartridgeRom, false}
};

[[nodiscard]] constexpr AddressRegion decodeAddress(std::uint16_t address) noexcept {
    for (const auto& region : memoryMap) {
        if (address >= region.begin && address <= region.end) return region.region;
    }
    return AddressRegion::unmapped;
}

} // namespace nex8::hardware
