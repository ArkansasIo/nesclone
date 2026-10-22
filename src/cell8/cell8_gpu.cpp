#include "nex8/gpu/cell8_gpu.hpp"

namespace nex8::gpu {

void Cell8Gpu::writeRegister(Register reg, std::uint8_t value) noexcept {
    const auto address = static_cast<std::uint16_t>(reg);
    if (address >= 0x2A00 && address <= 0x2A0F) {
        registers_[static_cast<std::size_t>(address - 0x2A00)] = value;
        if (reg == Register::paletteIndex) registers_[8] = value;
        if (reg == Register::paletteData) palette_[registers_[8]++] = value;
    }
}

std::uint8_t Cell8Gpu::readRegister(Register reg) const noexcept {
    const auto address = static_cast<std::uint16_t>(reg);
    if (address == static_cast<std::uint16_t>(Register::status)) {
        return static_cast<std::uint8_t>(vblank_ ? 0x80 : 0x00);
    }
    if (address >= 0x2A00 && address <= 0x2A0F) return registers_[address - 0x2A00];
    return 0;
}

void Cell8Gpu::writePalette(std::uint8_t index, std::uint8_t color) noexcept { palette_[index] = color; }
std::uint8_t Cell8Gpu::readPalette(std::uint8_t index) const noexcept { return palette_[index]; }

void Cell8Gpu::writeSprite(std::size_t index, Sprite sprite) noexcept {
    if (index < sprites_.size()) sprites_[index] = sprite;
}
Sprite Cell8Gpu::readSprite(std::size_t index) const noexcept {
    return index < sprites_.size() ? sprites_[index] : Sprite{};
}
void Cell8Gpu::writeVram(std::uint16_t address, std::uint8_t value) noexcept {
    vram_[static_cast<std::size_t>(address) % vram_.size()] = value;
}
std::uint8_t Cell8Gpu::readVram(std::uint16_t address) const noexcept {
    return vram_[static_cast<std::size_t>(address) % vram_.size()];
}

void Cell8Gpu::beginFrame() noexcept {
    frameCycles_ = 0;
    vblank_ = false;
}
void Cell8Gpu::tick(std::uint32_t cycles) noexcept {
    frameCycles_ += cycles;
    constexpr std::uint32_t targetCycles = 8'000'000U / 60U;
    if (frameCycles_ >= targetCycles) {
        frameCycles_ %= targetCycles;
        vblank_ = true;
        ++frameCount_;
    }
}
void Cell8Gpu::plot(std::size_t x, std::size_t y, std::uint8_t color) noexcept {
    if (x < width && y < height) framebuffer_[y * width + x] = color;
}

} // namespace nex8::gpu
