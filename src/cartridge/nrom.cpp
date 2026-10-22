#include "nes/cartridge/nrom.hpp"
#include <utility>

namespace nes {
Nrom::Nrom(const CartridgeImage& image)
    : prg_(image.prgRom),
      chr_(image.chrRom.empty() ? image.chrRam : image.chrRom),
      chrRam_(image.chrRom.empty()) {}

bool Nrom::cpuRead(u16 address, u8& value) const {
    if (address < 0x8000 || prg_.empty()) return false;
    std::size_t index = static_cast<std::size_t>(address - 0x8000);
    if (prg_.size() == 0x4000) index &= 0x3FFF;
    if (index >= prg_.size()) return false;
    value = prg_[index];
    return true;
}
bool Nrom::cpuWrite(u16 address, u8) { return address >= 0x8000; }
bool Nrom::ppuRead(u16 address, u8& value) const {
    if (address >= 0x2000 || chr_.empty()) return false;
    value = chr_[static_cast<std::size_t>(address) % chr_.size()];
    return true;
}
bool Nrom::ppuWrite(u16 address, u8 value) {
    if (address >= 0x2000 || !chrRam_ || chr_.empty()) return false;
    chr_[static_cast<std::size_t>(address) % chr_.size()] = value;
    return true;
}
}
