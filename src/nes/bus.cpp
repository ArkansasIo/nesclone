#include "nes/nes/bus.hpp"
#include "nes/apu/apu.hpp"
#include "nes/cartridge/mapper.hpp"
#include "nes/controller/controller.hpp"
#include <algorithm>

namespace nes {
Bus::Bus() = default;
void Bus::connectMapper(std::shared_ptr<Mapper> mapper) { mapper_ = std::move(mapper); }
void Bus::connectController(Controller* controller) { controller_ = controller; }
void Bus::connectApu(APU* apu) { apu_ = apu; }
void Bus::resetRam() { ram_.fill(0); ppuRegisters_.fill(0); apuIoRegisters_.fill(0); }
u8 Bus::read(u16 address) {
    if (address <= 0x1FFF) return ram_[address & 0x07FFu];
    if (address <= 0x3FFF) return ppuRegisters_[address & 0x0007u];
    if (address == 0x4016) return controller_ ? controller_->read() : 1;
    if (address == 0x4017) return 1;
    if (address >= 0x4000 && address <= 0x401F) return apuIoRegisters_[address - 0x4000];
    if (address >= 0x4020 && mapper_) {
        u8 value = 0;
        if (mapper_->cpuRead(address, value)) return value;
    }
    return 0;
}
void Bus::write(u16 address, u8 value) {
    if (address <= 0x1FFF) { ram_[address & 0x07FFu] = value; return; }
    if (address <= 0x3FFF) { ppuRegisters_[address & 0x0007u] = value; return; }
    if (address == 0x4016) {
        if (controller_) controller_->writeStrobe(value);
        apuIoRegisters_[address - 0x4000] = value;
        return;
    }
    if (address >= 0x4000 && address <= 0x401F)
    {
        apuIoRegisters_[address - 0x4000] = value;
        if (apu_) apu_->write(address, value);
        return;
    }
    if (address >= 0x4020 && mapper_) (void)mapper_->cpuWrite(address, value);
}
}
