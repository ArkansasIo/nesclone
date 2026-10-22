#include "nes/ppu/ppu.hpp"
namespace nes {
void PPU::reset() {
    nametableRam_.fill(0); paletteRam_.fill(0); oam_.fill(0); registers_.fill(0);
    v_=t_=0; fineX_=0; writeToggle_=false; readBuffer_=0; nmiPending_=false;
    scanline_=dot_=0;
}
u8 PPU::cpuRead(u16 address) {
    const u16 reg = static_cast<u16>(0x2000u | (address & 7u));
    if (reg == 0x2002) {
        const u8 value = registers_[2];
        registers_[2] &= 0x7Fu;
        writeToggle_ = false;
        nmiPending_ = false;
        return value;
    }
    if (reg == 0x2004) return oam_[registers_[3]];
    if (reg == 0x2007) {
        const u16 a = static_cast<u16>(v_ & 0x3FFFu);
        const u8 result = readBuffer_;
        readBuffer_ = ppuRead(a);
        v_ = static_cast<u16>((v_ + ((registers_[0] & 4u) ? 32u : 1u)) & 0x7FFFu);
        return result;
    }
    return registers_[reg & 7u];
}
void PPU::cpuWrite(u16 address, u8 value) {
    const u16 reg = static_cast<u16>(0x2000u | (address & 7u));
    registers_[reg & 7u] = value;
    switch (reg) {
    case 0x2000: t_ = static_cast<u16>((t_ & 0xF3FFu) | ((value & 3u) << 10u)); break;
    case 0x2003: break;
    case 0x2004: oam_[registers_[3]++] = value; break;
    case 0x2005:
        if (!writeToggle_) { fineX_ = value & 7u; t_ = static_cast<u16>((t_ & 0x7FE0u) | (value >> 3u)); }
        else { t_ = static_cast<u16>((t_ & 0x0C1Fu) | ((value & 0xF8u) << 2u) | ((value & 7u) << 12u)); }
        writeToggle_ = !writeToggle_; break;
    case 0x2006:
        if (!writeToggle_) t_ = static_cast<u16>((t_ & 0x00FFu) | ((value & 0x3Fu) << 8u));
        else { t_ = static_cast<u16>((t_ & 0x7F00u) | value); v_ = t_; }
        writeToggle_ = !writeToggle_; break;
    case 0x2007: ppuWrite(static_cast<u16>(v_ & 0x3FFFu), value); v_ = static_cast<u16>((v_ + ((registers_[0] & 4u) ? 32u : 1u)) & 0x7FFFu); break;
    default: break;
    }
}
u8 PPU::ppuRead(u16 address) const {
    address &= 0x3FFFu;
    if (address < 0x2000) return 0; // cartridge CHR routes in the full system
    if (address < 0x3F00) return nametableRam_[(address - 0x2000u) & 0x0FFFu];
    u16 i = static_cast<u16>((address - 0x3F00u) & 0x1Fu);
    if ((i & 0x13u) == 0x10u) i = static_cast<u16>(i - 0x10u);
    return paletteRam_[i];
}
void PPU::ppuWrite(u16 address, u8 value) {
    address &= 0x3FFFu;
    if (address >= 0x2000 && address < 0x3F00) nametableRam_[(address - 0x2000u) & 0x0FFFu] = value;
    else if (address >= 0x3F00) {
        u16 i = static_cast<u16>((address - 0x3F00u) & 0x1Fu);
        if ((i & 0x13u) == 0x10u) i = static_cast<u16>(i - 0x10u);
        paletteRam_[i] = value;
    }
}
void PPU::clock() {
    if (scanline_ == 241 && dot_ == 1) {
        registers_[2] |= 0x80u;
        if (registers_[0] & 0x80u) nmiPending_ = true;
    }
    if (scanline_ == 261 && dot_ == 1) registers_[2] &= 0x1Fu;
    if (++dot_ > 340) { dot_ = 0; scanline_ = static_cast<u16>((scanline_ + 1u) % 262u); }
}
}
