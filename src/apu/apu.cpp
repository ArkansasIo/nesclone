#include "nes/apu/apu.hpp"
namespace nes {
u8 APU::read(u16 address) {
    if (address == 0x4015) return static_cast<u8>((status_ & 0x1Fu) | (registers_[0x15] & 0xE0u));
    if (address >= 0x4000 && address <= 0x4017) return registers_[address - 0x4000];
    return 0;
}
void APU::write(u16 address, u8 value) {
    if (address < 0x4000 || address > 0x4017) return;
    registers_[address - 0x4000] = value;
    if (address == 0x4015) status_ = static_cast<u8>(value & 0x1Fu);
}
void APU::clock() {}
void APU::reset() { registers_.fill(0); status_ = 0; }
}
