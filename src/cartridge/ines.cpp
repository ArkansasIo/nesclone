#include "nes/cartridge/ines.hpp"
#include <array>
#include <fstream>
#include <limits>

namespace nes {
namespace {
bool readExact(std::ifstream& in, std::vector<u8>& out, std::size_t size) {
    out.resize(size);
    if (size == 0) return true;
    in.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(size));
    return in.good() || static_cast<std::size_t>(in.gcount()) == size;
}
std::size_t decodeRomSize(u8 low, u8 highNibble, std::size_t unit, bool nes2) {
    if (nes2 && highNibble == 0x0F) {
        const unsigned exponent = low >> 2;
        const unsigned multiplier = (low & 0x03u) * 2u + 1u;
        if (exponent >= sizeof(std::size_t) * 8u) return 0;
        return (static_cast<std::size_t>(1) << exponent) * multiplier;
    }
    const std::size_t banks = static_cast<std::size_t>(low) |
        (nes2 ? (static_cast<std::size_t>(highNibble) << 8u) : 0u);
    if (banks > std::numeric_limits<std::size_t>::max() / unit) return 0;
    return banks * unit;
}
}
LoadResult loadInes(const std::filesystem::path& path) {
    LoadResult result{};
    std::ifstream in(path, std::ios::binary);
    if (!in) { result.error = "Could not open ROM: " + path.string(); return result; }
    std::array<u8,16> h{};
    in.read(reinterpret_cast<char*>(h.data()), static_cast<std::streamsize>(h.size()));
    if (in.gcount() != static_cast<std::streamsize>(h.size())) {
        result.error = "ROM is shorter than the 16-byte iNES header"; return result;
    }
    if (h[0] != 'N' || h[1] != 'E' || h[2] != 'S' || h[3] != 0x1A) {
        result.error = "Invalid iNES magic signature"; return result;
    }
    auto& image = result.image;
    image.nes2 = (h[7] & 0x0C) == 0x08;
    image.trainerPresent = (h[6] & 0x04) != 0;
    image.battery = (h[6] & 0x02) != 0;
    image.fourScreen = (h[6] & 0x08) != 0;
    image.verticalMirroring = (h[6] & 0x01) != 0;
    image.mapperId = static_cast<u16>((h[6] >> 4) | (h[7] & 0xF0));
    if (image.nes2) {
        image.mapperId = static_cast<u16>(image.mapperId | ((h[8] & 0x0F) << 8));
        image.submapper = static_cast<u8>(h[8] >> 4);
        image.prgRamBytes = h[10] == 0 ? 0 : (64u << (h[10] >> 4));
    } else {
        if (h[7] & 0x08) { result.error = "Unsupported archaic iNES header variant"; return result; }
        image.prgRamBytes = h[8] == 0 ? 8192 : static_cast<std::size_t>(h[8]) * 8192;
    }
    if (image.trainerPresent) {
        if (!readExact(in, image.trainer, 512)) { result.error = "Truncated trainer"; return result; }
    }
    const auto prgSize = decodeRomSize(h[4], h[9] & 0x0F, 16384, image.nes2);
    const auto chrSize = decodeRomSize(h[5], h[9] >> 4, 8192, image.nes2);
    if (prgSize == 0 || prgSize > 64u * 1024u * 1024u) {
        result.error = "Invalid or unsupported PRG-ROM size"; return result;
    }
    if (chrSize > 64u * 1024u * 1024u) {
        result.error = "Unsupported CHR-ROM size"; return result;
    }
    if (!readExact(in, image.prgRom, prgSize)) { result.error = "Truncated PRG-ROM data"; return result; }
    if (chrSize && !readExact(in, image.chrRom, chrSize)) { result.error = "Truncated CHR-ROM data"; return result; }
    if (chrSize == 0) image.chrRam.resize(8192);
    return result;
}
}
