#include "nex8/input/input_actions.hpp"

#include <algorithm>
#include <string>

namespace nex8::input {

void ActionMap::bind(Action action, Binding binding) {
    bindings_[action].push_back(binding);
}

void ActionMap::clear(Action action) { bindings_.erase(action); }

const std::vector<Binding>& ActionMap::bindings(Action action) const {
    static const std::vector<Binding> empty;
    const auto it = bindings_.find(action);
    return it == bindings_.end() ? empty : it->second;
}

bool ActionMap::down(Action action, const InputSystem& input) const {
    for (const auto& b : bindings(action)) {
        switch (b.type) {
        case BindingType::Key:
            if (input.keyDown(b.key)) return true;
            break;
        case BindingType::MouseButton:
            if (input.mouseDown(b.mouseButton)) return true;
            break;
        case BindingType::GamepadButton:
            if (input.gamepadDown(b.gamepad, b.gamepadButton)) return true;
            break;
        case BindingType::GamepadAxis: {
            const float v = input.gamepadAxis(b.gamepad, b.gamepadAxis);
            if ((b.positive && v >= b.threshold) || (!b.positive && v <= -b.threshold)) return true;
            break;
        }
        }
    }
    return false;
}

bool ActionMap::pressed(Action action, const InputSystem& input) const {
    for (const auto& b : bindings(action)) {
        if (b.type == BindingType::Key && input.keyPressed(b.key)) return true;
    }
    return false;
}

float ActionMap::axis(Action action, const InputSystem& input) const {
    float result = 0.0f;
    for (const auto& b : bindings(action)) {
        switch (b.type) {
        case BindingType::Key:
            if (input.keyDown(b.key)) result += b.positive ? 1.0f : -1.0f;
            break;
        case BindingType::GamepadAxis: {
            const float v = input.gamepadAxis(b.gamepad, b.gamepadAxis);
            result += b.positive ? std::max(0.0f, v) : std::min(0.0f, v);
            break;
        }
        default:
            break;
        }
    }
    return std::clamp(result, -1.0f, 1.0f);
}

std::string ActionMap::actionName(Action action) const {
    switch (action) {
    case Action::MoveUp: return "MoveUp";
    case Action::MoveDown: return "MoveDown";
    case Action::MoveLeft: return "MoveLeft";
    case Action::MoveRight: return "MoveRight";
    case Action::Fire: return "Fire";
    case Action::Jump: return "Jump";
    case Action::Select: return "Select";
    case Action::Start: return "Start";
    case Action::Pause: return "Pause";
    case Action::Menu: return "Menu";
    case Action::CameraUp: return "CameraUp";
    case Action::CameraDown: return "CameraDown";
    case Action::CameraLeft: return "CameraLeft";
    case Action::CameraRight: return "CameraRight";
    case Action::MousePrimary: return "MousePrimary";
    case Action::MouseSecondary: return "MouseSecondary";
    }
    return "Unknown";
}

ActionMap makeDefaultActionMap() {
    ActionMap map;
    map.bind(Action::MoveUp, {BindingType::Key, Key::Up});
    map.bind(Action::MoveUp, {BindingType::Key, Key::W});
    map.bind(Action::MoveDown, {BindingType::Key, Key::Down});
    map.bind(Action::MoveDown, {BindingType::Key, Key::S});
    map.bind(Action::MoveLeft, {BindingType::Key, Key::Left});
    map.bind(Action::MoveLeft, {BindingType::Key, Key::A});
    map.bind(Action::MoveRight, {BindingType::Key, Key::Right});
    map.bind(Action::MoveRight, {BindingType::Key, Key::D});
    map.bind(Action::Fire, {BindingType::Key, Key::Z});
    map.bind(Action::Fire, {BindingType::MouseButton, Key::Unknown, MouseButton::Left});
    map.bind(Action::Jump, {BindingType::Key, Key::Space});
    map.bind(Action::Select, {BindingType::Key, Key::Shift});
    map.bind(Action::Start, {BindingType::Key, Key::Enter});
    map.bind(Action::Pause, {BindingType::Key, Key::Escape});
    map.bind(Action::Menu, {BindingType::Key, Key::Tab});
    map.bind(Action::MousePrimary, {BindingType::MouseButton, Key::Unknown, MouseButton::Left});
    map.bind(Action::MouseSecondary, {BindingType::MouseButton, Key::Unknown, MouseButton::Right});
    map.bind(Action::MoveUp, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::DPadUp});
    map.bind(Action::MoveDown, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::DPadDown});
    map.bind(Action::MoveLeft, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::DPadLeft});
    map.bind(Action::MoveRight, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::DPadRight});
    map.bind(Action::Fire, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::A});
    map.bind(Action::Jump, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::B});
    map.bind(Action::Start, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::Start});
    return map;
}

} // namespace nex8::input
