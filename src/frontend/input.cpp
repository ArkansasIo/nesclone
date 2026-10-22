#include "nes/frontend/input.hpp"
namespace nes {
void InputMapper::set(HostKey key, bool down, Controller& controller) const {
    Button button = Button::A;
    switch (key) {
    case HostKey::Z: button = Button::A; break;
    case HostKey::X: button = Button::B; break;
    case HostKey::RightShift: button = Button::Select; break;
    case HostKey::Enter: button = Button::Start; break;
    case HostKey::Up: button = Button::Up; break;
    case HostKey::Down: button = Button::Down; break;
    case HostKey::Left: button = Button::Left; break;
    case HostKey::Right: button = Button::Right; break;
    }
    controller.setButton(button, down);
}
}
