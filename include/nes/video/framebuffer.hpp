#pragma once
#include "nes/types.hpp"
#include <array>

namespace nes {
struct Rgba { u8 r=0, g=0, b=0, a=255; };
class Framebuffer {
public:
    static constexpr std::size_t Width = 256;
    static constexpr std::size_t Height = 240;
    void clear(Rgba color = {});
    void setPixel(std::size_t x, std::size_t y, Rgba color);
    [[nodiscard]] const std::array<Rgba, Width * Height>& pixels() const { return pixels_; }
private:
    std::array<Rgba, Width * Height> pixels_{};
};
}
