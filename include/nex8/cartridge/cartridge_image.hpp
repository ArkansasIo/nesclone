#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

namespace nex8::cartridge {

inline constexpr std::uint8_t magic[] = {'N', '8', 'C', 'R'};

struct Header final {
    std::uint8_t version{1};
    std::uint8_t flags{};
    std::uint16_t mapper{};
    std::uint32_t prgBytes{};
    std::uint32_t graphicsBytes{};
    std::uint32_t audioBytes{};
    std::uint32_t dataBytes{};
    std::uint16_t entryPoint{0x8000};
};

struct Image final {
    Header header;
    std::vector<std::uint8_t> prg;
    std::vector<std::uint8_t> graphics;
    std::vector<std::uint8_t> audio;
    std::vector<std::uint8_t> data;
};

struct LoadResult final {
    bool ok{};
    Image image;
    std::string error;
};

[[nodiscard]] LoadResult parse(std::span<const std::uint8_t> bytes);
[[nodiscard]] LoadResult load(const std::filesystem::path& path);
[[nodiscard]] std::vector<std::uint8_t> pack(const Image& image, std::string& error);

} // namespace nex8::cartridge
