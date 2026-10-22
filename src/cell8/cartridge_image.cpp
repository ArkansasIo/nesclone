#include "nex8/cartridge/cartridge_image.hpp"

#include <array>
#include <fstream>
#include <iterator>
#include <limits>

namespace nex8::cartridge {
namespace {
constexpr std::size_t headerSize = 24;
void put16(std::vector<std::uint8_t>& b, std::size_t p, std::uint16_t v) {
    b[p] = static_cast<std::uint8_t>(v & 0xFFU);
    b[p + 1] = static_cast<std::uint8_t>((v >> 8U) & 0xFFU);
}
void put32(std::vector<std::uint8_t>& b, std::size_t p, std::uint32_t v) {
    for (std::size_t i = 0; i < 4; ++i) b[p + i] = static_cast<std::uint8_t>((v >> (i * 8U)) & 0xFFU);
}
std::uint16_t get16(std::span<const std::uint8_t> b, std::size_t p) {
    return static_cast<std::uint16_t>(b[p] | (static_cast<std::uint16_t>(b[p + 1]) << 8U));
}
std::uint32_t get32(std::span<const std::uint8_t> b, std::size_t p) {
    std::uint32_t value = 0;
    for (std::size_t i = 0; i < 4; ++i) value |= static_cast<std::uint32_t>(b[p + i]) << (i * 8U);
    return value;
}
}

LoadResult parse(std::span<const std::uint8_t> bytes) {
    if (bytes.size() < headerSize) return {false, {}, "N8CR image is shorter than its 24-byte header"};
    for (std::size_t i = 0; i < 4; ++i) if (bytes[i] != magic[i]) return {false, {}, "Invalid N8CR magic"};
    if (bytes[4] != 1) return {false, {}, "Unsupported N8CR format version"};
    Header h;
    h.version = bytes[4]; h.flags = bytes[5]; h.mapper = get16(bytes, 6);
    h.prgBytes = get32(bytes, 8); h.graphicsBytes = get32(bytes, 12);
    h.audioBytes = get32(bytes, 16); h.dataBytes = get32(bytes, 20);
    // Avoid overflow and reject truncated/trailing data.
    std::uint64_t total = headerSize;
    total += h.prgBytes; total += h.graphicsBytes; total += h.audioBytes; total += h.dataBytes;
    if (total != bytes.size()) return {false, {}, "N8CR section lengths do not match file length"};
    Image image; image.header = h;
    std::size_t offset = headerSize;
    const auto readSection = [&](std::vector<std::uint8_t>& out, std::uint32_t size, std::size_t& at) {
        out.assign(bytes.begin() + static_cast<std::ptrdiff_t>(at),
                   bytes.begin() + static_cast<std::ptrdiff_t>(at + size));
        at += size;
    };
    readSection(image.prg, h.prgBytes, offset);
    readSection(image.graphics, h.graphicsBytes, offset);
    readSection(image.audio, h.audioBytes, offset);
    readSection(image.data, h.dataBytes, offset);
    return {true, std::move(image), {}};
}

LoadResult load(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {false, {}, "Unable to open cartridge image: " + path.string()};
    std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(file)), {});
    return parse(bytes);
}

std::vector<std::uint8_t> pack(const Image& image, std::string& error) {
    const auto sectionFits = [](std::size_t n) { return n <= std::numeric_limits<std::uint32_t>::max(); };
    if (!sectionFits(image.prg.size()) || !sectionFits(image.graphics.size()) ||
        !sectionFits(image.audio.size()) || !sectionFits(image.data.size())) {
        error = "Cartridge section is too large for N8CR version 1";
        return {};
    }
    std::vector<std::uint8_t> bytes(headerSize, 0);
    for (std::size_t i = 0; i < 4; ++i) bytes[i] = magic[i];
    bytes[4] = 1; bytes[5] = image.header.flags; put16(bytes, 6, image.header.mapper);
    put32(bytes, 8, static_cast<std::uint32_t>(image.prg.size()));
    put32(bytes, 12, static_cast<std::uint32_t>(image.graphics.size()));
    put32(bytes, 16, static_cast<std::uint32_t>(image.audio.size()));
    put32(bytes, 20, static_cast<std::uint32_t>(image.data.size()));
    bytes.insert(bytes.end(), image.prg.begin(), image.prg.end());
    bytes.insert(bytes.end(), image.graphics.begin(), image.graphics.end());
    bytes.insert(bytes.end(), image.audio.begin(), image.audio.end());
    bytes.insert(bytes.end(), image.data.begin(), image.data.end());
    error.clear();
    return bytes;
}

} // namespace nex8::cartridge
