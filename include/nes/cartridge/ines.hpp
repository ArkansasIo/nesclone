#pragma once
#include "nes/types.hpp"
#include <filesystem>
#include <string>
#include <vector>

namespace nes {
struct CartridgeImage {
    u16 mapperId = 0;
    u8 submapper = 0;
    bool battery = false;
    bool trainerPresent = false;
    bool fourScreen = false;
    bool verticalMirroring = false;
    bool nes2 = false;
    std::size_t prgRamBytes = 0;
    std::vector<u8> trainer;
    std::vector<u8> prgRom;
    std::vector<u8> chrRom;
    std::vector<u8> chrRam;

    [[nodiscard]] bool hasChrRam() const { return chrRom.empty() && !chrRam.empty(); }
};

struct LoadResult {
    CartridgeImage image;
    std::string error;
    [[nodiscard]] explicit operator bool() const { return error.empty(); }
};

[[nodiscard]] LoadResult loadInes(const std::filesystem::path& path);
}
