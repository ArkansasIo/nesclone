#include "nes/debugger/debugger.hpp"
#include "nes/cpu/cpu6502.hpp"
#include "nes/nes/bus.hpp"
#include <iomanip>
#include <sstream>
namespace nes {
std::string Debugger::registers(const Cpu6502& cpu) const {
    const auto& r = cpu.registers();
    std::ostringstream out;
    out << std::hex << std::uppercase << std::setfill('0')
        << "A=$" << std::setw(2) << static_cast<unsigned>(r.a)
        << " X=$" << std::setw(2) << static_cast<unsigned>(r.x)
        << " Y=$" << std::setw(2) << static_cast<unsigned>(r.y)
        << " SP=$" << std::setw(2) << static_cast<unsigned>(r.sp)
        << " PC=$" << std::setw(4) << static_cast<unsigned>(r.pc)
        << " P=$" << std::setw(2) << static_cast<unsigned>(r.p)
        << " cycles=" << std::dec << cpu.cycles();
    return out.str();
}
std::vector<u8> Debugger::readMemory(const Bus& bus, u16 address, std::size_t count) const {
    std::vector<u8> result; result.reserve(count);
    for (std::size_t i=0; i<count; ++i) result.push_back(const_cast<Bus&>(bus).read(static_cast<u16>(address + i)));
    return result;
}
}
