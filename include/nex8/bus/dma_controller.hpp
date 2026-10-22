#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace nex8::bus {

enum class DmaTarget : std::uint8_t { videoRam, paletteRam, spriteRam, spuLocalStore, audioBuffer };

struct DmaRequest final {
    std::uint16_t source{};
    std::uint16_t destination{};
    std::uint16_t length{};
    DmaTarget target{DmaTarget::videoRam};
};

struct DmaResult final {
    bool ok{};
    std::uint16_t bytesTransferred{};
    std::string error;
};

class DmaController final {
public:
    [[nodiscard]] DmaResult transfer(std::span<const std::uint8_t> source,
                                     std::span<std::uint8_t> destination,
                                     DmaRequest request) const;
};

} // namespace nex8::bus
