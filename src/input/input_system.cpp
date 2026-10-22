#include "nex8/input/input_system.hpp"

#include <algorithm>
#include <cmath>

namespace nex8::input {

namespace {
std::size_t index(MouseButton button) { return static_cast<std::size_t>(button); }
std::size_t index(GamepadButton button) { return static_cast<std::size_t>(button); }
std::size_t index(GamepadAxis axis) { return static_cast<std::size_t>(axis); }
}

void InputSystem::beginFrame(std::uint64_t frame) {
    previous_ = frame_;
    frame_.frame = frame;
    frame_.mouse.deltaX = 0;
    frame_.mouse.deltaY = 0;
    frame_.mouse.wheel = 0;
}

void InputSystem::setKey(Key key, bool down) { frame_.keys[key] = down; }

void InputSystem::setMousePosition(std::int32_t x, std::int32_t y) {
    frame_.mouse.deltaX += x - frame_.mouse.x;
    frame_.mouse.deltaY += y - frame_.mouse.y;
    frame_.mouse.x = x;
    frame_.mouse.y = y;
}

void InputSystem::setMouseButton(MouseButton button, bool down) {
    frame_.mouse.buttons[index(button)] = down;
}

void InputSystem::addMouseWheel(std::int32_t delta) { frame_.mouse.wheel += delta; }

void InputSystem::setGamepadConnected(std::size_t i, bool connected) {
    if (i < frame_.gamepads.size()) frame_.gamepads[i].connected = connected;
}

void InputSystem::setGamepadButton(std::size_t i, GamepadButton button, bool down) {
    if (i < frame_.gamepads.size()) frame_.gamepads[i].buttons[index(button)] = down;
}

void InputSystem::setGamepadAxis(std::size_t i, GamepadAxis axis, float value) {
    if (i < frame_.gamepads.size()) frame_.gamepads[i].axes[index(axis)] = std::clamp(value, -1.0f, 1.0f);
}

bool InputSystem::keyDown(Key key) const {
    const auto it = frame_.keys.find(key);
    return it != frame_.keys.end() && it->second;
}

bool InputSystem::keyPressed(Key key) const {
    const auto now = keyDown(key);
    const auto it = previous_.keys.find(key);
    const auto before = it != previous_.keys.end() && it->second;
    return now && !before;
}

bool InputSystem::mouseDown(MouseButton button) const { return frame_.mouse.buttons[index(button)]; }

bool InputSystem::mousePressed(MouseButton button) const {
    return mouseDown(button) && !previous_.mouse.buttons[index(button)];
}

bool InputSystem::gamepadDown(std::size_t i, GamepadButton button) const {
    return i < frame_.gamepads.size() && frame_.gamepads[i].connected && frame_.gamepads[i].buttons[index(button)];
}

bool InputSystem::gamepadPressed(std::size_t i, GamepadButton button) const {
    if (i >= frame_.gamepads.size() || !frame_.gamepads[i].connected) return false;
    const bool now = frame_.gamepads[i].buttons[index(button)];
    const bool before = previous_.gamepads[i].connected && previous_.gamepads[i].buttons[index(button)];
    return now && !before;
}

float InputSystem::gamepadAxis(std::size_t i, GamepadAxis axis) const {
    if (i >= frame_.gamepads.size() || !frame_.gamepads[i].connected) return 0.0f;
    return frame_.gamepads[i].axes[index(axis)];
}

void InputSystem::mapToConsole(nes::Controller& controller, std::size_t gamepad) const {
    const bool up = keyDown(Key::Up) || gamepadDown(gamepad, GamepadButton::DPadUp) || gamepadAxis(gamepad, GamepadAxis::LeftY) < -0.5f;
    const bool down = keyDown(Key::Down) || gamepadDown(gamepad, GamepadButton::DPadDown) || gamepadAxis(gamepad, GamepadAxis::LeftY) > 0.5f;
    const bool left = keyDown(Key::Left) || gamepadDown(gamepad, GamepadButton::DPadLeft) || gamepadAxis(gamepad, GamepadAxis::LeftX) < -0.5f;
    const bool right = keyDown(Key::Right) || gamepadDown(gamepad, GamepadButton::DPadRight) || gamepadAxis(gamepad, GamepadAxis::LeftX) > 0.5f;
    const bool a = keyDown(Key::Z) || gamepadDown(gamepad, GamepadButton::A) || mouseDown(MouseButton::Left);
    const bool b = keyDown(Key::X) || gamepadDown(gamepad, GamepadButton::B) || mouseDown(MouseButton::Right);
    const bool select = keyDown(Key::Shift) || gamepadDown(gamepad, GamepadButton::Back);
    const bool start = keyDown(Key::Enter) || keyDown(Key::Space) || gamepadDown(gamepad, GamepadButton::Start);

    controller.setButton(nes::Button::Up, up);
    controller.setButton(nes::Button::Down, down);
    controller.setButton(nes::Button::Left, left);
    controller.setButton(nes::Button::Right, right);
    controller.setButton(nes::Button::A, a);
    controller.setButton(nes::Button::B, b);
    controller.setButton(nes::Button::Select, select);
    controller.setButton(nes::Button::Start, start);
}

} // namespace nex8::input
