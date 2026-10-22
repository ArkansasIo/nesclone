#pragma once

#include <array>
#include <cstdint>
#include <span>

namespace nex8::gpu {

enum class Register : std::uint16_t {
    control = 0x2A00, status = 0x2A01,
    backgroundX = 0x2A02, backgroundY = 0x2A03,
    spriteX = 0x2A04, spriteY = 0x2A05,
    spriteTile = 0x2A06, spriteAttr = 0x2A07,
    paletteIndex = 0x2A08, paletteData = 0x2A09,
    dmaSourceLow = 0x2A0A, dmaSourceHigh = 0x2A0B,
    dmaDestLow = 0x2A0C, dmaDestHigh = 0x2A0D,
    dmaLengthLow = 0x2A0E, dmaLengthHigh = 0x2A0F
};

struct Sprite final {
    std::uint8_t x{}, y{}, tile{}, attributes{};
};

class Cell8Gpu final {
public:
    static constexpr std::size_t width = 256;
    static constexpr std::size_t height = 240;
    static constexpr std::size_t pixels = width * height;
    static constexpr std::size_t tileBytes = 16;
    static constexpr std::size_t maxSprites = 64;

    void writeRegister(Register reg, std::uint8_t value) noexcept;
    [[nodiscard]] std::uint8_t readRegister(Register reg) const noexcept;
    void writePalette(std::uint8_t index, std::uint8_t color) noexcept;
    [[nodiscard]] std::uint8_t readPalette(std::uint8_t index) const noexcept;
    void writeSprite(std::size_t index, Sprite sprite) noexcept;
    [[nodiscard]] Sprite readSprite(std::size_t index) const noexcept;
    void writeVram(std::uint16_t address, std::uint8_t value) noexcept;
    [[nodiscard]] std::uint8_t readVram(std::uint16_t address) const noexcept;
    void beginFrame() noexcept;
    void tick(std::uint32_t cycles) noexcept;
    [[nodiscard]] bool vblank() const noexcept { return vblank_; }
    [[nodiscard]] std::uint64_t frameCount() const noexcept { return frameCount_; }
    [[nodiscard]] std::span<const std::uint8_t> framebuffer() const noexcept { return framebuffer_; }
    void plot(std::size_t x, std::size_t y, std::uint8_t color) noexcept;

private:
    std::array<std::uint8_t, 16> registers_{};
    std::array<std::uint8_t, 256> palette_{};
    std::array<Sprite, maxSprites> sprites_{};
    std::array<std::uint8_t, 2048> vram_{};
    std::array<std::uint8_t, pixels> framebuffer_{};
    std::uint32_t frameCycles_{};
    std::uint64_t frameCount_{};
    bool vblank_{};
};

} // namespace nex8::gpu
