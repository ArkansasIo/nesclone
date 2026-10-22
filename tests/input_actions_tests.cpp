#include "nex8/input/input_actions.hpp"

#include <cassert>

int main() {
    nex8::input::InputSystem input;
    auto actions = nex8::input::makeDefaultActionMap();

    input.beginFrame(1);
    input.setKey(nex8::input::Key::W, true);
    input.setKey(nex8::input::Key::Z, true);
    input.setMouseButton(nex8::input::MouseButton::Left, true);

    assert(actions.down(nex8::input::Action::MoveUp, input));
    assert(actions.down(nex8::input::Action::Fire, input));
    assert(actions.pressed(nex8::input::Action::Fire, input));
    assert(actions.axis(nex8::input::Action::MoveUp, input) == 1.0f);

    input.beginFrame(2);
    assert(actions.down(nex8::input::Action::MoveUp, input));
    assert(!actions.pressed(nex8::input::Action::Fire, input));

    input.setGamepadConnected(0, true);
    input.setGamepadButton(0, nex8::input::GamepadButton::A, true);
    assert(actions.down(nex8::input::Action::Fire, input));
    return 0;
}
