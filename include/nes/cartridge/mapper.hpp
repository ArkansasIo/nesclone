#pragma once
#include "nes/types.hpp"

namespace nes {
class Mapper {
public:
    virtual ~Mapper() = default;
    [[nodiscard]] virtual bool cpuRead(u16 address, u8& value) const = 0;
    virtual bool cpuWrite(u16 address, u8 value) = 0;
    [[nodiscard]] virtual bool ppuRead(u16 address, u8& value) const = 0;
    virtual bool ppuWrite(u16 address, u8 value) = 0;
    virtual void reset() {}
};
}
