#include "nex8/input/input_profile.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string_view>
#include <vector>

namespace nex8::input {
namespace {

std::vector<std::string> split(std::string_view value, char delimiter) {
    std::vector<std::string> parts;
    std::string current;
    for (const char ch : value) {
        if (ch == delimiter) {
            parts.push_back(current);
            current.clear();
        } else {
            current += ch;
        }
    }
    parts.push_back(current);
    return parts;
}

std::string escape(std::string value) {
    std::string result;
    result.reserve(value.size());
    for (const char ch : value) {
        if (ch == '\\' || ch == '|') result += '\\';
        result += ch;
    }
    return result;
}

std::string actionName(Action action) { return ActionMap{}.actionName(action); }

std::optional<Action> actionFromName(std::string_view name) {
    static const std::vector<Action> all = {
        Action::MoveUp, Action::MoveDown, Action::MoveLeft, Action::MoveRight,
        Action::Fire, Action::Jump, Action::Select, Action::Start, Action::Pause, Action::Menu,
        Action::CameraUp, Action::CameraDown, Action::CameraLeft, Action::CameraRight,
        Action::MousePrimary, Action::MouseSecondary
    };
    const ActionMap names;
    for (const auto action : all) if (names.actionName(action) == name) return action;
    return std::nullopt;
}

std::string bindingTypeName(BindingType type) {
    switch (type) {
    case BindingType::Key: return "key";
    case BindingType::MouseButton: return "mouse";
    case BindingType::GamepadButton: return "gamepad_button";
    case BindingType::GamepadAxis: return "gamepad_axis";
    }
    return "key";
}

std::optional<BindingType> bindingTypeFromName(std::string_view name) {
    if (name == "key") return BindingType::Key;
    if (name == "mouse") return BindingType::MouseButton;
    if (name == "gamepad_button") return BindingType::GamepadButton;
    if (name == "gamepad_axis") return BindingType::GamepadAxis;
    return std::nullopt;
}

template <typename Enum>
std::string enumNumber(Enum value) { return std::to_string(static_cast<unsigned>(value)); }

bool parseUnsigned(std::string_view text, unsigned& value) {
    try {
        std::size_t used = 0;
        const auto parsed = std::stoul(std::string(text), &used);
        if (used != text.size()) return false;
        value = static_cast<unsigned>(parsed);
        return true;
    } catch (...) { return false; }
}

bool parseFloat(std::string_view text, float& value) {
    try {
        std::size_t used = 0;
        value = std::stof(std::string(text), &used);
        return used == text.size();
    } catch (...) { return false; }
}

} // namespace

InputProfile makeDefaultInputProfile() { return InputProfile{}; }

InputProfile makeKeyboardMouseProfile() {
    auto profile = makeDefaultInputProfile();
    profile.name = "Keyboard + Mouse";
    profile.gamepadDeadZone = 0.20f;
    return profile;
}

InputProfile makeModernGamepadProfile() {
    auto profile = makeDefaultInputProfile();
    profile.name = "Modern Gamepad";
    profile.actions.clear(Action::MoveUp);
    profile.actions.clear(Action::MoveDown);
    profile.actions.clear(Action::MoveLeft);
    profile.actions.clear(Action::MoveRight);
    profile.actions.bind(Action::MoveUp, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::DPadUp, GamepadAxis::LeftX, 0.5f, true, 0});
    profile.actions.bind(Action::MoveDown, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::DPadDown, GamepadAxis::LeftX, 0.5f, true, 0});
    profile.actions.bind(Action::MoveLeft, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::DPadLeft, GamepadAxis::LeftX, 0.5f, true, 0});
    profile.actions.bind(Action::MoveRight, {BindingType::GamepadButton, Key::Unknown, MouseButton::Left, GamepadButton::DPadRight, GamepadAxis::LeftX, 0.5f, true, 0});
    profile.actions.bind(Action::MoveLeft, {BindingType::GamepadAxis, Key::Unknown, MouseButton::Left, GamepadButton::A, GamepadAxis::LeftX, 0.25f, false, 0});
    profile.actions.bind(Action::MoveRight, {BindingType::GamepadAxis, Key::Unknown, MouseButton::Left, GamepadButton::A, GamepadAxis::LeftX, 0.25f, true, 0});
    profile.actions.bind(Action::MoveUp, {BindingType::GamepadAxis, Key::Unknown, MouseButton::Left, GamepadButton::A, GamepadAxis::LeftY, 0.25f, false, 0});
    profile.actions.bind(Action::MoveDown, {BindingType::GamepadAxis, Key::Unknown, MouseButton::Left, GamepadButton::A, GamepadAxis::LeftY, 0.25f, true, 0});
    profile.gamepadDeadZone = 0.15f;
    profile.gamepadSensitivity = 1.0f;
    return profile;
}

InputProfile makeArcadeProfile() {
    auto profile = makeDefaultInputProfile();
    profile.name = "Arcade";
    profile.actions.clear(Action::Fire);
    profile.actions.bind(Action::Fire, {BindingType::Key, Key::Z});
    profile.actions.bind(Action::Fire, {BindingType::Key, Key::X});
    profile.actions.bind(Action::Start, {BindingType::Key, Key::Enter});
    return profile;
}

std::string serializeInputProfile(const InputProfile& profile) {
    std::ostringstream out;
    out << "# NEX8 input profile v1\n";
    out << "schema=" << profile.schemaVersion << '\n';
    out << "name=" << escape(profile.name) << '\n';
    out << std::setprecision(6);
    out << "mouse_sensitivity=" << profile.mouseSensitivity << '\n';
    out << "invert_mouse_y=" << (profile.invertMouseY ? 1 : 0) << '\n';
    out << "gamepad_dead_zone=" << profile.gamepadDeadZone << '\n';
    out << "gamepad_trigger_threshold=" << profile.gamepadTriggerThreshold << '\n';
    out << "gamepad_sensitivity=" << profile.gamepadSensitivity << '\n';
    out << "gamepad_index=" << profile.gamepadIndex << '\n';
    for (const auto action : {Action::MoveUp, Action::MoveDown, Action::MoveLeft, Action::MoveRight,
                              Action::Fire, Action::Jump, Action::Select, Action::Start, Action::Pause, Action::Menu,
                              Action::CameraUp, Action::CameraDown, Action::CameraLeft, Action::CameraRight,
                              Action::MousePrimary, Action::MouseSecondary}) {
        for (const auto& b : profile.actions.bindings(action)) {
            out << "binding=" << actionName(action) << '|' << bindingTypeName(b.type) << '|'
                << enumNumber(b.key) << '|' << enumNumber(b.mouseButton) << '|'
                << enumNumber(b.gamepadButton) << '|' << enumNumber(b.gamepadAxis) << '|'
                << b.threshold << '|' << (b.positive ? 1 : 0) << '|' << b.gamepad << '\n';
        }
    }
    return out.str();
}

InputConfigResult loadInputProfileText(InputProfile& profile, const std::string& text) {
    std::istringstream input(text);
    std::string line;
    ActionMap loaded;
    bool sawBinding = false;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') continue;
        const auto equals = line.find('=');
        if (equals == std::string::npos) return {false, "Invalid input profile line: " + line};
        const auto key = line.substr(0, equals);
        const auto value = line.substr(equals + 1);
        if (key == "schema") {
            unsigned v{};
            if (!parseUnsigned(value, v)) return {false, "Invalid schema value."};
            profile.schemaVersion = static_cast<int>(v);
        } else if (key == "name") {
            profile.name = value;
        } else if (key == "mouse_sensitivity") {
            if (!parseFloat(value, profile.mouseSensitivity)) return {false, "Invalid mouse sensitivity."};
        } else if (key == "invert_mouse_y") {
            profile.invertMouseY = value == "1" || value == "true";
        } else if (key == "gamepad_dead_zone") {
            if (!parseFloat(value, profile.gamepadDeadZone)) return {false, "Invalid gamepad dead zone."};
        } else if (key == "gamepad_trigger_threshold") {
            if (!parseFloat(value, profile.gamepadTriggerThreshold)) return {false, "Invalid trigger threshold."};
        } else if (key == "gamepad_sensitivity") {
            if (!parseFloat(value, profile.gamepadSensitivity)) return {false, "Invalid gamepad sensitivity."};
        } else if (key == "gamepad_index") {
            unsigned v{};
            if (!parseUnsigned(value, v)) return {false, "Invalid gamepad index."};
            profile.gamepadIndex = v;
        } else if (key == "binding") {
            const auto fields = split(value, '|');
            if (fields.size() != 9) return {false, "Invalid binding record."};
            const auto action = actionFromName(fields[0]);
            const auto type = bindingTypeFromName(fields[1]);
            if (!action || !type) return {false, "Unknown binding action or type."};
            unsigned keyValue{}, mouseValue{}, buttonValue{}, axisValue{}, gamepadValue{}, positiveValue{};
            float threshold{};
            if (!parseUnsigned(fields[2], keyValue) || !parseUnsigned(fields[3], mouseValue) ||
                !parseUnsigned(fields[4], buttonValue) || !parseUnsigned(fields[5], axisValue) ||
                !parseFloat(fields[6], threshold) || !parseUnsigned(fields[7], positiveValue) ||
                !parseUnsigned(fields[8], gamepadValue)) return {false, "Invalid binding values."};
            if (keyValue >= 0x100 || mouseValue >= 5 || buttonValue >= 15 || axisValue >= 6 || gamepadValue >= 4) {
                return {false, "Binding enum value is out of range."};
            }
            Binding binding;
            binding.type = *type;
            binding.key = static_cast<Key>(keyValue);
            binding.mouseButton = static_cast<MouseButton>(mouseValue);
            binding.gamepadButton = static_cast<GamepadButton>(buttonValue);
            binding.gamepadAxis = static_cast<GamepadAxis>(axisValue);
            binding.threshold = std::clamp(threshold, 0.0f, 1.0f);
            binding.positive = positiveValue != 0;
            binding.gamepad = gamepadValue;
            loaded.bind(*action, binding);
            sawBinding = true;
        }
    }
    if (sawBinding) profile.actions = std::move(loaded);
    profile.mouseSensitivity = std::max(0.0f, profile.mouseSensitivity);
    profile.gamepadDeadZone = std::clamp(profile.gamepadDeadZone, 0.0f, 0.99f);
    profile.gamepadTriggerThreshold = std::clamp(profile.gamepadTriggerThreshold, 0.0f, 1.0f);
    profile.gamepadSensitivity = std::max(0.0f, profile.gamepadSensitivity);
    return {true, {}};
}

InputConfigResult saveInputProfile(const InputProfile& profile, const std::filesystem::path& path) {
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) return {false, "Cannot open input profile for writing: " + path.string()};
    output << serializeInputProfile(profile);
    if (!output) return {false, "Failed to write input profile: " + path.string()};
    return {true, {}};
}

InputConfigResult loadInputProfile(InputProfile& profile, const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) return {false, "Cannot open input profile: " + path.string()};
    std::ostringstream contents;
    contents << input.rdbuf();
    return loadInputProfileText(profile, contents.str());
}

} // namespace nex8::input
