#pragma once
#include "nes/types.hpp"

namespace nes {
enum class Button : u8 {
    A = 0x01, B = 0x02, Select = 0x04, Start = 0x08,
    Up = 0x10, Down = 0x20, Left = 0x40, Right = 0x80
};

class Controller {
public:
    void setButton(Button button, bool pressed);
    void writeStrobe(u8 value);
    [[nodiscard]] u8 read();
    [[nodiscard]] u8 buttons() const { return buttons_; }
private:
    u8 buttons_ = 0;
    u8 shift_ = 0;
    bool strobe_ = false;
};
}
