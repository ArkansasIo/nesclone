#include "nes/controller/controller.hpp"
namespace nes {
void Controller::setButton(Button button, bool pressed) {
    const auto mask = static_cast<u8>(button);
    buttons_ = pressed ? static_cast<u8>(buttons_ | mask)
                       : static_cast<u8>(buttons_ & static_cast<u8>(~mask));
}
void Controller::writeStrobe(u8 value) {
    const bool next = (value & 1u) != 0;
    if (strobe_ && !next) shift_ = buttons_;
    strobe_ = next;
    if (strobe_) shift_ = buttons_;
}
u8 Controller::read() {
    if (strobe_) return static_cast<u8>(buttons_ & 1u);
    const u8 result = static_cast<u8>(shift_ & 1u);
    shift_ = static_cast<u8>((shift_ >> 1u) | 0x80u);
    return result;
}
}
