#pragma once
#include "nes/types.hpp"

namespace nes {
class Bus;

struct CpuRegisters {
    u8 a = 0;
    u8 x = 0;
    u8 y = 0;
    u8 sp = 0xFD;
    u8 p = 0x24;
    u16 pc = 0;
};

class Cpu6502 {
public:
    explicit Cpu6502(Bus& bus);
    void reset();
    void clock();
    void irq();
    void nmi();
    [[nodiscard]] const CpuRegisters& registers() const { return r_; }
    [[nodiscard]] u64 cycles() const { return cycles_; }
    [[nodiscard]] bool halted() const { return halted_; }

private:
    Bus& bus_;
    CpuRegisters r_{};
    u64 cycles_ = 0;
    u8 remainingCycles_ = 0;
    bool halted_ = false;

    [[nodiscard]] u8 read(u16 address);
    void write(u16 address, u8 value);
    [[nodiscard]] u8 fetch();
    [[nodiscard]] u16 fetch16();
    void push(u8 value);
    [[nodiscard]] u8 pop();
    void setZN(u8 value);
    [[nodiscard]] bool flag(u8 mask) const;
    void setFlag(u8 mask, bool value);
    void serviceInterrupt(u16 vector, bool breakFlag);
    void execute(u8 opcode);
};
}
