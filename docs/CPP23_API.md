# C++23 Host SDK API

The C++23 SDK is the desktop-side development layer. It supports simulation, game tools and tests; it is not itself linked into the 6502 ROM.

## Objects

Header: `include/nes/sdk/game_object.hpp`

- `GameObject`: base class with lifecycle hooks and enabled state.
- `Paddle`: bounded vertical movement.
- `Ball`: deterministic fixed-tick motion and velocity changes.
- `Vec2i`: integer coordinates that make host simulation straightforward to test.

Header: `include/nes/sdk/pong_rules.hpp`

- `Score`: two-player score model.
- `overlapsAabb`: axis-aligned rectangle overlap test.
- `clampPaddle`: paddle-boundary helper.

## Example: object-oriented logic

```cpp
nes::sdk::Ball ball;
nes::sdk::Paddle player{"Player", 16, 104};

player.move(-2, 16, 200);
ball.update(1);

if (ball.position().y < 8 || ball.position().y > 232) {
    ball.reverseY();
}
```

## Logic statements

The separate `neslogic::Statement` tree provides `Action`, `Block` and `IfStatement`; expression nodes include literals, variables and comparison helpers. These classes are useful for deterministic host-side gameplay rules. They are not yet a compiler that emits machine code.
