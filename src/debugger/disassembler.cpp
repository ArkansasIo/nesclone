#include "nes/debugger/disassembler.hpp"
#include <iomanip>
#include <sstream>
namespace nes {
DisassemblyLine disassemble6502(u16 address, const std::vector<u8>& bytes) {
    DisassemblyLine line{address, bytes, "DB"};
    if (bytes.empty()) { line.text = "<no data>"; return line; }
    std::ostringstream out;
    out << "DB $" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<unsigned>(bytes[0]);
    line.text = out.str();
    return line; // Replace with full opcode/addressing table in assembler milestone.
}
}
