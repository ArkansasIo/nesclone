#include "nes/cpu/cpu6502.hpp"
#include "nes/nes/bus.hpp"

namespace nes {
namespace {
constexpr u8 CARRY = 0x01, ZERO = 0x02, IRQ_DISABLE = 0x04, DECIMAL = 0x08;
constexpr u8 BREAK_FLAG = 0x10, UNUSED = 0x20, OVERFLOW = 0x40, NEGATIVE = 0x80;
}
Cpu6502::Cpu6502(Bus& bus) : bus_(bus) {}
u8 Cpu6502::read(u16 a) { return bus_.read(a); }
void Cpu6502::write(u16 a, u8 v) { bus_.write(a, v); }
u8 Cpu6502::fetch() { return read(r_.pc++); }
u16 Cpu6502::fetch16() { const u8 lo = fetch(); const u8 hi = fetch(); return static_cast<u16>(lo | (static_cast<u16>(hi) << 8)); }
void Cpu6502::push(u8 value) { write(static_cast<u16>(0x0100u | r_.sp), value); --r_.sp; }
u8 Cpu6502::pop() { ++r_.sp; return read(static_cast<u16>(0x0100u | r_.sp)); }
bool Cpu6502::flag(u8 mask) const { return (r_.p & mask) != 0; }
void Cpu6502::setFlag(u8 mask, bool value) { r_.p = value ? static_cast<u8>(r_.p | mask) : static_cast<u8>(r_.p & static_cast<u8>(~mask)); r_.p |= UNUSED; }
void Cpu6502::setZN(u8 v) { setFlag(ZERO, v == 0); setFlag(NEGATIVE, (v & 0x80) != 0); }
void Cpu6502::reset() {
    r_ = {}; r_.sp = 0xFD; r_.p = static_cast<u8>(IRQ_DISABLE | UNUSED);
    halted_ = false; remainingCycles_ = 7; cycles_ = 0;
    const u8 lo = read(0xFFFC); const u8 hi = read(0xFFFD);
    r_.pc = static_cast<u16>(lo | (static_cast<u16>(hi) << 8));
}
void Cpu6502::serviceInterrupt(u16 vector, bool breakFlag) {
    push(static_cast<u8>(r_.pc >> 8)); push(static_cast<u8>(r_.pc & 0xFF));
    u8 pushed = static_cast<u8>((r_.p | UNUSED) & static_cast<u8>(~BREAK_FLAG));
    if (breakFlag) pushed |= BREAK_FLAG;
    push(pushed); setFlag(IRQ_DISABLE, true);
    const u8 lo = read(vector); const u8 hi = read(static_cast<u16>(vector + 1));
    r_.pc = static_cast<u16>(lo | (static_cast<u16>(hi) << 8)); remainingCycles_ = 7;
}
void Cpu6502::irq() { if (!flag(IRQ_DISABLE)) serviceInterrupt(0xFFFE, false); }
void Cpu6502::nmi() { serviceInterrupt(0xFFFA, false); }
void Cpu6502::clock() {
    if (halted_) { ++cycles_; return; }
    if (remainingCycles_ > 0) { --remainingCycles_; ++cycles_; return; }
    execute(fetch()); ++cycles_;
    if (remainingCycles_ > 0) --remainingCycles_;
}
void Cpu6502::execute(u8 op) {
    auto setCycles = [this](u8 n) { remainingCycles_ = n; };
    auto absX = [this]() { return static_cast<u16>(fetch16() + r_.x); };
    auto absY = [this]() { return static_cast<u16>(fetch16() + r_.y); };
    auto zpX = [this]() { return static_cast<u8>(fetch() + r_.x); };
    auto zpY = [this]() { return static_cast<u8>(fetch() + r_.y); };
    auto readAbs = [this](u16 a) { return read(a); };
    auto branch = [this, &setCycles](bool take) {
        const auto offset = static_cast<s8>(fetch());
        setCycles(2);
        if (take) {
            const u16 old = r_.pc;
            r_.pc = static_cast<u16>(r_.pc + offset);
            setCycles(static_cast<u8>(3 + ((old & 0xFF00) != (r_.pc & 0xFF00))));
        }
    };
    switch (op) {
    case 0x00: ++r_.pc; serviceInterrupt(0xFFFE, true); break; // BRK
    case 0xEA: setCycles(2); break; // NOP
    case 0xA9: r_.a = fetch(); setZN(r_.a); setCycles(2); break;
    case 0xA5: r_.a = read(fetch()); setZN(r_.a); setCycles(3); break;
    case 0xB5: r_.a = read(zpX()); setZN(r_.a); setCycles(4); break;
    case 0xAD: r_.a = read(fetch16()); setZN(r_.a); setCycles(4); break;
    case 0xBD: { const u16 a=absX(); r_.a=read(a); setZN(r_.a); setCycles(4); break; }
    case 0xB9: { const u16 a=absY(); r_.a=read(a); setZN(r_.a); setCycles(4); break; }
    case 0xA1: { const u8 z=static_cast<u8>(fetch()+r_.x); const u16 a=static_cast<u16>(read(z)|(static_cast<u16>(read(static_cast<u8>(z+1)))<<8)); r_.a=read(a); setZN(r_.a); setCycles(6); break; }
    case 0xB1: { const u8 z=fetch(); const u16 a=static_cast<u16>(read(z)|(static_cast<u16>(read(static_cast<u8>(z+1)))<<8)); r_.a=read(static_cast<u16>(a+r_.y)); setZN(r_.a); setCycles(5); break; }
    case 0xA2: r_.x=fetch(); setZN(r_.x); setCycles(2); break;
    case 0xA6: r_.x=read(fetch()); setZN(r_.x); setCycles(3); break;
    case 0xB6: r_.x=read(zpY()); setZN(r_.x); setCycles(4); break;
    case 0xAE: r_.x=read(fetch16()); setZN(r_.x); setCycles(4); break;
    case 0xBE: r_.x=read(absY()); setZN(r_.x); setCycles(4); break;
    case 0xA0: r_.y=fetch(); setZN(r_.y); setCycles(2); break;
    case 0xA4: r_.y=read(fetch()); setZN(r_.y); setCycles(3); break;
    case 0xB4: r_.y=read(zpX()); setZN(r_.y); setCycles(4); break;
    case 0xAC: r_.y=read(fetch16()); setZN(r_.y); setCycles(4); break;
    case 0xBC: r_.y=read(absX()); setZN(r_.y); setCycles(4); break;

    case 0x85: write(fetch(), r_.a); setCycles(3); break;
    case 0x95: write(zpX(), r_.a); setCycles(4); break;
    case 0x8D: write(fetch16(), r_.a); setCycles(4); break;
    case 0x9D: write(absX(), r_.a); setCycles(5); break;
    case 0x99: write(absY(), r_.a); setCycles(5); break;
    case 0x81: { const u8 z=static_cast<u8>(fetch()+r_.x); write(static_cast<u16>(read(z)|(static_cast<u16>(read(static_cast<u8>(z+1)))<<8)), r_.a); setCycles(6); break; }
    case 0x91: { const u8 z=fetch(); const u16 a=static_cast<u16>(read(z)|(static_cast<u16>(read(static_cast<u8>(z+1)))<<8)); write(static_cast<u16>(a+r_.y), r_.a); setCycles(6); break; }
    case 0x86: write(fetch(), r_.x); setCycles(3); break;
    case 0x96: write(zpY(), r_.x); setCycles(4); break;
    case 0x8E: write(fetch16(), r_.x); setCycles(4); break;
    case 0x84: write(fetch(), r_.y); setCycles(3); break;
    case 0x94: write(zpX(), r_.y); setCycles(4); break;
    case 0x8C: write(fetch16(), r_.y); setCycles(4); break;

    case 0xAA: r_.x=r_.a; setZN(r_.x); setCycles(2); break;
    case 0xA8: r_.y=r_.a; setZN(r_.y); setCycles(2); break;
    case 0x8A: r_.a=r_.x; setZN(r_.a); setCycles(2); break;
    case 0x98: r_.a=r_.y; setZN(r_.a); setCycles(2); break;
    case 0xBA: r_.x=r_.sp; setZN(r_.x); setCycles(2); break;
    case 0x9A: r_.sp=r_.x; setCycles(2); break;
    case 0xE8: ++r_.x; setZN(r_.x); setCycles(2); break;
    case 0xCA: --r_.x; setZN(r_.x); setCycles(2); break;
    case 0xC8: ++r_.y; setZN(r_.y); setCycles(2); break;
    case 0x88: --r_.y; setZN(r_.y); setCycles(2); break;

    case 0x69: { const u8 v=fetch(); const u16 sum=static_cast<u16>(r_.a)+v+(flag(CARRY)?1u:0u); const u8 out=static_cast<u8>(sum); setFlag(CARRY,sum>0xFF); setFlag(OVERFLOW,((~(r_.a^v)&(r_.a^out)&0x80)!=0)); r_.a=out; setZN(r_.a); setCycles(2); break; }
    case 0xE9: case 0xEB: { const u8 v=static_cast<u8>(~fetch()); const u16 sum=static_cast<u16>(r_.a)+v+(flag(CARRY)?1u:0u); const u8 out=static_cast<u8>(sum); setFlag(CARRY,sum>0xFF); setFlag(OVERFLOW,((~(r_.a^v)&(r_.a^out)&0x80)!=0)); r_.a=out; setZN(r_.a); setCycles(2); break; }
    case 0x29: r_.a &= fetch(); setZN(r_.a); setCycles(2); break;
    case 0x25: r_.a &= read(fetch()); setZN(r_.a); setCycles(3); break;
    case 0x2D: r_.a &= read(fetch16()); setZN(r_.a); setCycles(4); break;
    case 0x09: r_.a |= fetch(); setZN(r_.a); setCycles(2); break;
    case 0x05: r_.a |= read(fetch()); setZN(r_.a); setCycles(3); break;
    case 0x0D: r_.a |= read(fetch16()); setZN(r_.a); setCycles(4); break;
    case 0x49: r_.a ^= fetch(); setZN(r_.a); setCycles(2); break;
    case 0x45: r_.a ^= read(fetch()); setZN(r_.a); setCycles(3); break;
    case 0x4D: r_.a ^= read(fetch16()); setZN(r_.a); setCycles(4); break;

    case 0xC9: case 0xC5: case 0xCD: { const u8 v=op==0xC9?fetch():(op==0xC5?read(fetch()):read(fetch16())); const u8 out=static_cast<u8>(r_.a-v); setFlag(CARRY,r_.a>=v); setZN(out); setCycles(op==0xC9?2:(op==0xC5?3:4)); break; }
    case 0xE0: case 0xE4: case 0xEC: { const u8 v=op==0xE0?fetch():(op==0xE4?read(fetch()):read(fetch16())); const u8 out=static_cast<u8>(r_.x-v); setFlag(CARRY,r_.x>=v); setZN(out); setCycles(op==0xE0?2:(op==0xE4?3:4)); break; }
    case 0xC0: case 0xC4: case 0xCC: { const u8 v=op==0xC0?fetch():(op==0xC4?read(fetch()):read(fetch16())); const u8 out=static_cast<u8>(r_.y-v); setFlag(CARRY,r_.y>=v); setZN(out); setCycles(op==0xC0?2:(op==0xC4?3:4)); break; }

    case 0xE6: { const u8 a=fetch(); const u8 v=static_cast<u8>(read(a)+1); write(a,v); setZN(v); setCycles(5); break; }
    case 0xF6: { const u8 a=zpX(); const u8 v=static_cast<u8>(read(a)+1); write(a,v); setZN(v); setCycles(6); break; }
    case 0xEE: { const u16 a=fetch16(); const u8 v=static_cast<u8>(read(a)+1); write(a,v); setZN(v); setCycles(6); break; }
    case 0xFE: { const u16 a=absX(); const u8 v=static_cast<u8>(read(a)+1); write(a,v); setZN(v); setCycles(7); break; }
    case 0xC6: { const u8 a=fetch(); const u8 v=static_cast<u8>(read(a)-1); write(a,v); setZN(v); setCycles(5); break; }
    case 0xD6: { const u8 a=zpX(); const u8 v=static_cast<u8>(read(a)-1); write(a,v); setZN(v); setCycles(6); break; }
    case 0xCE: { const u16 a=fetch16(); const u8 v=static_cast<u8>(read(a)-1); write(a,v); setZN(v); setCycles(6); break; }
    case 0xDE: { const u16 a=absX(); const u8 v=static_cast<u8>(read(a)-1); write(a,v); setZN(v); setCycles(7); break; }

    case 0x0A: setFlag(CARRY,(r_.a&0x80)!=0); r_.a=static_cast<u8>(r_.a<<1); setZN(r_.a); setCycles(2); break;
    case 0x4A: setFlag(CARRY,(r_.a&1)!=0); r_.a=static_cast<u8>(r_.a>>1); setZN(r_.a); setCycles(2); break;
    case 0x2A: { const bool c=flag(CARRY); setFlag(CARRY,(r_.a&0x80)!=0); r_.a=static_cast<u8>((r_.a<<1)|(c?1:0)); setZN(r_.a); setCycles(2); break; }
    case 0x6A: { const bool c=flag(CARRY); setFlag(CARRY,(r_.a&1)!=0); r_.a=static_cast<u8>((r_.a>>1)|(c?0x80:0)); setZN(r_.a); setCycles(2); break; }

    case 0x18: setFlag(CARRY,false); setCycles(2); break;
    case 0x38: setFlag(CARRY,true); setCycles(2); break;
    case 0x58: setFlag(IRQ_DISABLE,false); setCycles(2); break;
    case 0x78: setFlag(IRQ_DISABLE,true); setCycles(2); break;
    case 0xB8: setFlag(OVERFLOW,false); setCycles(2); break;
    case 0xD8: setFlag(DECIMAL,false); setCycles(2); break;
    case 0xF8: setFlag(DECIMAL,true); setCycles(2); break;

    case 0x4C: r_.pc=fetch16(); setCycles(3); break;
    case 0x6C: { const u16 ptr=fetch16(); const u8 lo=read(ptr); const u16 hiAddr=static_cast<u16>((ptr&0xFF00)|((ptr+1)&0x00FF)); r_.pc=static_cast<u16>(lo|(static_cast<u16>(read(hiAddr))<<8)); setCycles(5); break; }
    case 0x20: { const u16 target=fetch16(); const u16 ret=static_cast<u16>(r_.pc-1); push(static_cast<u8>(ret>>8)); push(static_cast<u8>(ret)); r_.pc=target; setCycles(6); break; }
    case 0x60: { const u8 lo=pop(); const u8 hi=pop(); r_.pc=static_cast<u16>((lo|(static_cast<u16>(hi)<<8))+1); setCycles(6); break; }
    case 0x40: { r_.p=static_cast<u8>((pop()|UNUSED)&static_cast<u8>(~BREAK_FLAG)); const u8 lo=pop(); const u8 hi=pop(); r_.pc=static_cast<u16>(lo|(static_cast<u16>(hi)<<8)); setCycles(6); break; }
    case 0x48: push(r_.a); setCycles(3); break;
    case 0x68: r_.a=pop(); setZN(r_.a); setCycles(4); break;
    case 0x08: push(static_cast<u8>(r_.p|BREAK_FLAG|UNUSED)); setCycles(3); break;
    case 0x28: r_.p=static_cast<u8>((pop()|UNUSED)&static_cast<u8>(~BREAK_FLAG)); setCycles(4); break;

    case 0x10: branch(!flag(NEGATIVE)); break;
    case 0x30: branch(flag(NEGATIVE)); break;
    case 0x50: branch(!flag(OVERFLOW)); break;
    case 0x70: branch(flag(OVERFLOW)); break;
    case 0x90: branch(!flag(CARRY)); break;
    case 0xB0: branch(flag(CARRY)); break;
    case 0xD0: branch(!flag(ZERO)); break;
    case 0xF0: branch(flag(ZERO)); break;

    default: halted_ = true; setCycles(1); break;
    }
}
}
