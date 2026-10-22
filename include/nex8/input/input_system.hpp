#pragma once

#include "nes/controller/controller.hpp"

#include <array>
#include <cstdint>
#include <unordered_map>

namespace nex8::input {

enum class Key : std::uint16_t {
    Unknown, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Up, Down, Left, Right, Enter, Escape, Space, Tab, Shift, Ctrl, Alt, Backspace,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
};

enum class MouseButton : std::uint8_t { Left, Right, Middle, X1, X2 };

enum class GamepadButton : std::uint8_t {
    A, B, X, Y, Back, Start, Guide, LeftShoulder, RightShoulder,
    LeftStick, RightStick, DPadUp, DPadDown, DPadLeft, DPadRight
};

enum class GamepadAxis : std::uint8_t { LeftX, LeftY, RightX, RightY, LeftTrigger, RightTrigger };

struct MouseState final {
    std::int32_t x{};
    std::int32_t y{};
    std::int32_t deltaX{};
    std::int32_t deltaY{};
    std::int32_t wheel{};
    std::array<bool, 5> buttons{};
};

struct GamepadState final {
    bool connected{};
    std::array<bool, 15> buttons{};
    std::array<float, 6> axes{};
};

struct InputFrame final {
    std::uint64_t frame{};
    std::unordered_map<Key, bool> keys;
    MouseState mouse;
    std::array<GamepadState, 4> gamepads{};
    std::array<bool, 8> consoleButtons{};
};

class InputSystem final {
public:
    void beginFrame(std::uint64_t frame);
    void setKey(Key key, bool down);
    void setMousePosition(std::int32_t x, std::int32_t y);
    void setMouseButton(MouseButton button, bool down);
    void addMouseWheel(std::int32_t delta);
    void setGamepadConnected(std::size_t index, bool connected);
    void setGamepadButton(std::size_t index, GamepadButton button, bool down);
    void setGamepadAxis(std::size_t index, GamepadAxis axis, float value);

    [[nodiscard]] bool keyDown(Key key) const;
    [[nodiscard]] bool keyPressed(Key key) const;
    [[nodiscard]] bool mouseDown(MouseButton button) const;
    [[nodiscard]] bool mousePressed(MouseButton button) const;
    [[nodiscard]] bool gamepadDown(std::size_t index, GamepadButton button) const;
    [[nodiscard]] bool gamepadPressed(std::size_t index, GamepadButton button) const;
    [[nodiscard]] float gamepadAxis(std::size_t index, GamepadAxis axis) const;
    [[nodiscard]] const MouseState& mouse() const noexcept { return frame_.mouse; }
    [[nodiscard]] const InputFrame& frame() const noexcept { return frame_; }

    // Maps desktop/gamepad input into the 8-button NEX8 console controller.
    void mapToConsole(nes::Controller& controller, std::size_t gamepad = 0) const;

private:
    InputFrame frame_{};
    InputFrame previous_{};
};

} // namespace nex8::input
