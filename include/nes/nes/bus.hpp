#pragma once
#include "nes/types.hpp"
#include <array>
#include <memory>

namespace nes {
class Mapper;
class Controller;

class Bus {
public:
    Bus();
    void connectMapper(std::shared_ptr<Mapper> mapper);
    void connectController(Controller* controller);
    [[nodiscard]] u8 read(u16 address);
    void write(u16 address, u8 value);
    void resetRam();
private:
    std::array<u8, 2048> ram_{};
    std::array<u8, 8> ppuRegisters_{};
    std::array<u8, 0x18> apuIoRegisters_{};
    std::shared_ptr<Mapper> mapper_;
    Controller* controller_ = nullptr;
};
}
