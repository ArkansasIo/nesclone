#pragma once
#include "nes/types.hpp"
#include <array>

namespace nes {
// Interface and register/latch state only. Rendering pipeline is a later milestone.
class PPU {
public:
    [[nodiscard]] u8 cpuRead(u16 address);
    void cpuWrite(u16 address, u8 value);
    [[nodiscard]] u8 ppuRead(u16 address) const;
    void ppuWrite(u16 address, u8 value);
    void reset();
    void clock();
    [[nodiscard]] bool nmiRequested() const { return nmiPending_; }
private:
    std::array<u8, 4096> nametableRam_{};
    std::array<u8, 32> paletteRam_{};
    std::array<u8, 256> oam_{};
    std::array<u8, 8> registers_{};
    u16 v_ = 0, t_ = 0;
    u8 fineX_ = 0;
    bool writeToggle_ = false;
    u8 readBuffer_ = 0;
    bool nmiPending_ = false;
    u16 scanline_ = 0;
    u16 dot_ = 0;
};
}
