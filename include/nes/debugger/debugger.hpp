#pragma once
#include "nes/types.hpp"
#include <set>
#include <string>
#include <vector>

namespace nes {
class Bus;
class Cpu6502;
class Debugger {
public:
    void addBreakpoint(u16 address) { breakpoints_.insert(address); }
    void removeBreakpoint(u16 address) { breakpoints_.erase(address); }
    [[nodiscard]] bool hasBreakpoint(u16 address) const { return breakpoints_.contains(address); }
    [[nodiscard]] std::string registers(const Cpu6502& cpu) const;
    [[nodiscard]] std::vector<u8> readMemory(const Bus& bus, u16 address, std::size_t count) const;
private:
    std::set<u16> breakpoints_;
};
}
