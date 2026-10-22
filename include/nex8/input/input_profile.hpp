#pragma once

#include "nex8/input/input_actions.hpp"

#include <filesystem>
#include <string>

namespace nex8::input {

struct InputProfile final {
    int schemaVersion{1};
    std::string name{"NEX8 Default"};
    ActionMap actions{makeDefaultActionMap()};
    float mouseSensitivity{1.0f};
    bool invertMouseY{false};
    float gamepadDeadZone{0.20f};
    float gamepadTriggerThreshold{0.10f};
    float gamepadSensitivity{1.0f};
    std::size_t gamepadIndex{0};
};

struct InputConfigResult final {
    bool ok{false};
    std::string error;
};

[[nodiscard]] InputConfigResult saveInputProfile(const InputProfile& profile,
                                                  const std::filesystem::path& path);
[[nodiscard]] InputConfigResult loadInputProfile(InputProfile& profile,
                                                  const std::filesystem::path& path);
[[nodiscard]] InputConfigResult loadInputProfileText(InputProfile& profile,
                                                      const std::string& text);
[[nodiscard]] std::string serializeInputProfile(const InputProfile& profile);

[[nodiscard]] InputProfile makeDefaultInputProfile();
[[nodiscard]] InputProfile makeKeyboardMouseProfile();
[[nodiscard]] InputProfile makeModernGamepadProfile();
[[nodiscard]] InputProfile makeArcadeProfile();

} // namespace nex8::input
