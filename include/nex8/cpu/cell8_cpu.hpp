#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace nex8::cpu {

enum class Flag : std::uint8_t {
    carry = 0x01, zero = 0x02, interruptDisable = 0x04,
    decimal = 0x08, overflow = 0x40, negative = 0x80
};

struct Registers final {
    std::uint8_t a{}, b{}, c{}, d{}, e{}, f{}, x{}, y{};
    std::uint16_t pc{0x8000};
    std::uint16_t sp{0x1FFF};
    std::uint8_t flags{0x04};
};

enum class Opcode : std::uint8_t {
    nop = 0x00, halt = 0x01,
    ldiA = 0x10, ldiB = 0x11, ldiC = 0x12, ldiD = 0x13,
    ldiE = 0x14, ldiF = 0x15, ldiX = 0x16, ldiY = 0x17,
    ldaAbs = 0x20, staAbs = 0x21, ldbAbs = 0x22, stbAbs = 0x23,
    addAImm = 0x30, subAImm = 0x31, andAImm = 0x32, orAImm = 0x33,
    xorAImm = 0x34, cmpAImm = 0x35,
    incA = 0x38, decA = 0x39, incX = 0x3A, decX = 0x3B,
    jmpAbs = 0x40, jzAbs = 0x41, jnzAbs = 0x42, jcAbs = 0x43, jncAbs = 0x44,
    callAbs = 0x48, ret = 0x49, pushA = 0x4A, popA = 0x4B,
    inA = 0x50, outA = 0x51, wait = 0x52
};

struct StepResult final {
    bool ok{};
    bool halted{};
    std::uint8_t opcode{};
    std::uint8_t cycles{};
    std::string error;
};

class Cell8Cpu final {
public:
    explicit Cell8Cpu(std::size_t ramBytes = 65'536);
    void reset(std::uint16_t entry = 0x8000);
    [[nodiscard]] StepResult step();
    [[nodiscard]] StepResult run(std::uint64_t maxInstructions);

    [[nodiscard]] const Registers& registers() const noexcept { return regs_; }
    [[nodiscard]] Registers& registers() noexcept { return regs_; }
    [[nodiscard]] bool halted() const noexcept { return halted_; }
    [[nodiscard]] std::span<const std::uint8_t> memory() const noexcept { return memory_; }
    [[nodiscard]] std::span<std::uint8_t> memory() noexcept { return memory_; }

    [[nodiscard]] bool load(std::uint16_t address, std::span<const std::uint8_t> bytes);
    [[nodiscard]] std::uint8_t read8(std::uint16_t address) const noexcept;
    void write8(std::uint16_t address, std::uint8_t value) noexcept;
    [[nodiscard]] std::uint16_t read16(std::uint16_t address) const noexcept;

private:
    [[nodiscard]] std::uint8_t fetch8() noexcept;
    [[nodiscard]] std::uint16_t fetch16() noexcept;
    void setFlag(Flag flag, bool value) noexcept;
    [[nodiscard]] bool flag(Flag flag) const noexcept;
    void setNZ(std::uint8_t value) noexcept;
    [[nodiscard]] std::uint16_t fetchTarget() noexcept;
    [[nodiscard]] StepResult fail(std::uint8_t opcode, std::string message) const;

    Registers regs_{};
    std::vector<std::uint8_t> memory_;
    bool halted_{};
};

} // namespace nex8::cpu
