#pragma once

#include "nex8/input/input_system.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace nex8::input {

enum class Action : std::uint16_t {
    MoveUp, MoveDown, MoveLeft, MoveRight,
    Fire, Jump, Select, Start, Pause, Menu,
    CameraUp, CameraDown, CameraLeft, CameraRight,
    MousePrimary, MouseSecondary
};

enum class BindingType : std::uint8_t { Key, MouseButton, GamepadButton, GamepadAxis };

struct Binding final {
    BindingType type{BindingType::Key};
    Key key{Key::Unknown};
    MouseButton mouseButton{MouseButton::Left};
    GamepadButton gamepadButton{GamepadButton::A};
    GamepadAxis gamepadAxis{GamepadAxis::LeftX};
    float threshold{0.5f};
    bool positive{true};
    std::size_t gamepad{0};
};

class ActionMap final {
public:
    void bind(Action action, Binding binding);
    void clear(Action action);
    [[nodiscard]] bool down(Action action, const InputSystem& input) const;
    [[nodiscard]] bool pressed(Action action, const InputSystem& input) const;
    [[nodiscard]] float axis(Action action, const InputSystem& input) const;
    [[nodiscard]] const std::vector<Binding>& bindings(Action action) const;
    [[nodiscard]] std::string actionName(Action action) const;

private:
    std::unordered_map<Action, std::vector<Binding>> bindings_;
};

ActionMap makeDefaultActionMap();

} // namespace nex8::input
