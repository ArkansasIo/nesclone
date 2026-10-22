#pragma once
#include "nes/types.hpp"
#include <string>
#include <vector>

namespace nes {
struct DisassemblyLine { u16 address = 0; std::vector<u8> bytes; std::string text; };
[[nodiscard]] DisassemblyLine disassemble6502(u16 address, const std::vector<u8>& bytes);
}
