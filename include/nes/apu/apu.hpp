#pragma once
#include "nes/types.hpp"
#include <array>

namespace nes {
// CPU-visible register shell; audio channel synthesis is not yet implemented.
class APU {
public:
    [[nodiscard]] u8 read(u16 address);
    void write(u16 address, u8 value);
    void clock();
    void reset();
private:
    std::array<u8, 0x18> registers_{};
    u8 status_ = 0;
};
}
