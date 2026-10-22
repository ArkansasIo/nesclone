#pragma once
#include "nes/cartridge/ines.hpp"
#include "nes/cartridge/mapper.hpp"
#include <vector>

namespace nes {
class Nrom final : public Mapper {
public:
    explicit Nrom(const CartridgeImage& image);
    [[nodiscard]] bool cpuRead(u16 address, u8& value) const override;
    bool cpuWrite(u16 address, u8 value) override;
    [[nodiscard]] bool ppuRead(u16 address, u8& value) const override;
    bool ppuWrite(u16 address, u8 value) override;
    void reset() override {}
private:
    std::vector<u8> prg_;
    std::vector<u8> chr_;
    bool chrRam_ = false;
};
}
