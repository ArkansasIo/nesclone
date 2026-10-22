#include "nex8/input/input_profile.hpp"

#include <cassert>
#include <string>

int main() {
    using namespace nex8::input;

    InputProfile original = makeDefaultInputProfile();
    original.name = "Test Profile";
    original.mouseSensitivity = 1.75f;
    original.invertMouseY = true;
    original.gamepadDeadZone = 0.30f;
    original.gamepadTriggerThreshold = 0.20f;
    original.gamepadSensitivity = 1.25f;
    original.gamepadIndex = 2;

    const std::string text = serializeInputProfile(original);
    assert(text.find("name=Test Profile") != std::string::npos);
    assert(text.find("binding=MoveUp") != std::string::npos);

    InputProfile restored = makeKeyboardMouseProfile();
    const auto loaded = loadInputProfileText(restored, text);
    assert(loaded.ok);
    assert(restored.name == original.name);
    assert(restored.invertMouseY == original.invertMouseY);
    assert(restored.gamepadIndex == original.gamepadIndex);
    assert(restored.actions.bindings(Action::MoveUp).size() == original.actions.bindings(Action::MoveUp).size());
    assert(restored.actions.bindings(Action::Fire).size() == original.actions.bindings(Action::Fire).size());

    InputProfile gamepad = makeModernGamepadProfile();
    assert(gamepad.gamepadDeadZone < 0.20f);
    assert(!gamepad.actions.bindings(Action::MoveRight).empty());

    InputProfile arcade = makeArcadeProfile();
    assert(arcade.name == "Arcade");
    assert(arcade.actions.bindings(Action::Fire).size() == 2);

    InputProfile rejected = makeDefaultInputProfile();
    const auto bad = loadInputProfileText(rejected, "gamepad_index=9\n");
    assert(!bad.ok);

    return 0;
}
