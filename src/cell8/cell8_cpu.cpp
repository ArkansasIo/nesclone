#include "nex8/cpu/cell8_cpu.hpp"

#include <algorithm>
#include <limits>
#include <utility>

namespace nex8::cpu {

Cell8Cpu::Cell8Cpu(std::size_t ramBytes) : memory_(std::clamp<std::size_t>(ramBytes, 65'536, 65'536), 0) {
    reset();
}

void Cell8Cpu::reset(std::uint16_t entry) {
    regs_ = {};
    regs_.pc = entry;
    regs_.sp = 0x1FFF;
    regs_.flags = static_cast<std::uint8_t>(Flag::interruptDisable);
    halted_ = false;
}

std::uint8_t Cell8Cpu::read8(std::uint16_t address) const noexcept {
    return memory_[address];
}

void Cell8Cpu::write8(std::uint16_t address, std::uint8_t value) noexcept {
    memory_[address] = value;
}

std::uint16_t Cell8Cpu::read16(std::uint16_t address) const noexcept {
    const auto low = static_cast<std::uint16_t>(memory_[address]);
    const auto high = static_cast<std::uint16_t>(memory_[static_cast<std::uint16_t>(address + 1U)]);
    return static_cast<std::uint16_t>(low | static_cast<std::uint16_t>(high << 8U));
}

bool Cell8Cpu::load(std::uint16_t address, std::span<const std::uint8_t> bytes) {
    if (bytes.size() > memory_.size() - static_cast<std::size_t>(address)) return false;
    std::copy(bytes.begin(), bytes.end(), memory_.begin() + address);
    return true;
}

std::uint8_t Cell8Cpu::fetch8() noexcept {
    const auto value = read8(regs_.pc);
    regs_.pc = static_cast<std::uint16_t>(regs_.pc + 1U);
    return value;
}

std::uint16_t Cell8Cpu::fetch16() noexcept {
    const auto lo = static_cast<std::uint16_t>(fetch8());
    const auto hi = static_cast<std::uint16_t>(fetch8());
    return static_cast<std::uint16_t>(lo | static_cast<std::uint16_t>(hi << 8U));
}

std::uint16_t Cell8Cpu::fetchTarget() noexcept { return fetch16(); }

void Cell8Cpu::setFlag(Flag item, bool value) noexcept {
    const auto mask = static_cast<std::uint8_t>(item);
    if (value) regs_.flags = static_cast<std::uint8_t>(regs_.flags | mask);
    else regs_.flags = static_cast<std::uint8_t>(regs_.flags & static_cast<std::uint8_t>(~mask));
}

bool Cell8Cpu::flag(Flag item) const noexcept {
    return (regs_.flags & static_cast<std::uint8_t>(item)) != 0;
}

void Cell8Cpu::setNZ(std::uint8_t value) noexcept {
    setFlag(Flag::zero, value == 0);
    setFlag(Flag::negative, (value & 0x80U) != 0);
}

StepResult Cell8Cpu::fail(std::uint8_t opcode, std::string message) const {
    return {false, halted_, opcode, 0, std::move(message)};
}

StepResult Cell8Cpu::step() {
    if (halted_) return {true, true, 0, 0, {}};
    const auto opcodeByte = fetch8();
    const auto opcode = static_cast<Opcode>(opcodeByte);
    std::uint8_t cycles = 1;

    switch (opcode) {
    case Opcode::nop: cycles = 1; break;
    case Opcode::halt: halted_ = true; cycles = 1; break;
    case Opcode::ldiA: regs_.a = fetch8(); setNZ(regs_.a); cycles = 2; break;
    case Opcode::ldiB: regs_.b = fetch8(); setNZ(regs_.b); cycles = 2; break;
    case Opcode::ldiC: regs_.c = fetch8(); setNZ(regs_.c); cycles = 2; break;
    case Opcode::ldiD: regs_.d = fetch8(); setNZ(regs_.d); cycles = 2; break;
    case Opcode::ldiE: regs_.e = fetch8(); setNZ(regs_.e); cycles = 2; break;
    case Opcode::ldiF: regs_.f = fetch8(); setNZ(regs_.f); cycles = 2; break;
    case Opcode::ldiX: regs_.x = fetch8(); setNZ(regs_.x); cycles = 2; break;
    case Opcode::ldiY: regs_.y = fetch8(); setNZ(regs_.y); cycles = 2; break;
    case Opcode::ldaAbs: regs_.a = read8(fetchTarget()); setNZ(regs_.a); cycles = 4; break;
    case Opcode::staAbs: write8(fetchTarget(), regs_.a); cycles = 4; break;
    case Opcode::ldbAbs: regs_.b = read8(fetchTarget()); setNZ(regs_.b); cycles = 4; break;
    case Opcode::stbAbs: write8(fetchTarget(), regs_.b); cycles = 4; break;
    case Opcode::addAImm: {
        const auto operand = fetch8();
        const auto sum = static_cast<std::uint16_t>(regs_.a) + operand;
        const auto result = static_cast<std::uint8_t>(sum & 0xFFU);
        setFlag(Flag::carry, sum > 0xFFU);
        setFlag(Flag::overflow, ((~(regs_.a ^ operand) & (regs_.a ^ result)) & 0x80U) != 0);
        regs_.a = result; setNZ(regs_.a); cycles = 2; break;
    }
    case Opcode::subAImm: {
        const auto operand = fetch8();
        const auto lhs = static_cast<std::uint16_t>(regs_.a);
        const auto resultWide = static_cast<std::uint16_t>(lhs - operand);
        const auto result = static_cast<std::uint8_t>(resultWide & 0xFFU);
        setFlag(Flag::carry, lhs >= operand);
        setFlag(Flag::overflow, (((regs_.a ^ result) & (regs_.a ^ operand)) & 0x80U) != 0);
        regs_.a = result; setNZ(regs_.a); cycles = 2; break;
    }
    case Opcode::andAImm: regs_.a = static_cast<std::uint8_t>(regs_.a & fetch8()); setNZ(regs_.a); cycles = 2; break;
    case Opcode::orAImm: regs_.a = static_cast<std::uint8_t>(regs_.a | fetch8()); setNZ(regs_.a); cycles = 2; break;
    case Opcode::xorAImm: regs_.a = static_cast<std::uint8_t>(regs_.a ^ fetch8()); setNZ(regs_.a); cycles = 2; break;
    case Opcode::cmpAImm: {
        const auto value = fetch8();
        setFlag(Flag::carry, regs_.a >= value);
        setNZ(static_cast<std::uint8_t>(regs_.a - value));
        cycles = 2;
        break;
    }
    case Opcode::incA: ++regs_.a; setNZ(regs_.a); cycles = 1; break;
    case Opcode::decA: --regs_.a; setNZ(regs_.a); cycles = 1; break;
    case Opcode::incX: ++regs_.x; setNZ(regs_.x); cycles = 1; break;
    case Opcode::decX: --regs_.x; setNZ(regs_.x); cycles = 1; break;
    case Opcode::jmpAbs: regs_.pc = fetchTarget(); cycles = 3; break;
    case Opcode::jzAbs: { const auto target = fetchTarget(); if (flag(Flag::zero)) regs_.pc = target; cycles = 3; break; }
    case Opcode::jnzAbs: { const auto target = fetchTarget(); if (!flag(Flag::zero)) regs_.pc = target; cycles = 3; break; }
    case Opcode::jcAbs: { const auto target = fetchTarget(); if (flag(Flag::carry)) regs_.pc = target; cycles = 3; break; }
    case Opcode::jncAbs: { const auto target = fetchTarget(); if (!flag(Flag::carry)) regs_.pc = target; cycles = 3; break; }
    case Opcode::pushA:
        write8(regs_.sp, regs_.a);
        regs_.sp = static_cast<std::uint16_t>(regs_.sp - 1U);
        cycles = 2; break;
    case Opcode::popA:
        regs_.sp = static_cast<std::uint16_t>(regs_.sp + 1U);
        regs_.a = read8(regs_.sp);
        setNZ(regs_.a);
        cycles = 2; break;
    case Opcode::inA:
    case Opcode::outA:
    case Opcode::wait:
        return fail(opcodeByte, "I/O and WAIT opcodes require bus-device integration.");
    case Opcode::callAbs:
    case Opcode::ret:
        return fail(opcodeByte, "CALL/RET are reserved until stack/vector semantics are finalized.");
    default:
        return fail(opcodeByte, "Illegal or unsupported CELL8 opcode.");
    }

    return {true, halted_, opcodeByte, cycles, {}};
}

StepResult Cell8Cpu::run(std::uint64_t maxInstructions) {
    StepResult result{true, halted_, 0, 0, {}};
    for (std::uint64_t i = 0; i < maxInstructions; ++i) {
        result = step();
        if (!result.ok || result.halted) return result;
    }
    return result;
}

} // namespace nex8::cpu
