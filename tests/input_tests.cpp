#include "nex8/input/input_system.hpp"

#include <cassert>

int main() {
    nex8::input::InputSystem input;
    nes::Controller controller;

    input.beginFrame(1);
    input.setKey(nex8::input::Key::Z, true);
    input.setKey(nex8::input::Key::Right, true);
    input.setMousePosition(100, 50);
    input.setMouseButton(nex8::input::MouseButton::Left, true);
    input.mapToConsole(controller);
    assert(input.keyPressed(nex8::input::Key::Z));
    assert((controller.buttons() & static_cast<nes::u8>(nes::Button::A)) != 0);
    assert((controller.buttons() & static_cast<nes::u8>(nes::Button::Right)) != 0);

    input.beginFrame(2);
    assert(!input.keyPressed(nex8::input::Key::Z));
    assert(input.keyDown(nex8::input::Key::Z));

    input.setGamepadConnected(0, true);
    input.setGamepadButton(0, nex8::input::GamepadButton::B, true);
    input.setGamepadAxis(0, nex8::input::GamepadAxis::LeftY, -1.0f);
    input.mapToConsole(controller);
    assert((controller.buttons() & static_cast<nes::u8>(nes::Button::B)) != 0);
    assert((controller.buttons() & static_cast<nes::u8>(nes::Button::Up)) != 0);
    return 0;
}
