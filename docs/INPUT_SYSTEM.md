# NEX8 Input System

The NEX8 host runtime now exposes a device-independent action layer above keyboard, mouse, and gamepad input.

## Layers

1. `InputSystem` captures per-frame device state.
2. `ActionMap` maps physical controls to gameplay actions.
3. `InputProfile` stores action bindings and sensitivity/dead-zone settings.
4. Studio can use profiles as the source of truth for an eventual Controls/Input Mapping editor.
5. `mapToConsole()` can still translate desktop/gamepad input to the legacy eight-button `nes::Controller` interface.

## Actions

The initial action vocabulary includes:

- `MoveUp`, `MoveDown`, `MoveLeft`, `MoveRight`
- `Fire`, `Jump`, `Select`, `Start`, `Pause`, `Menu`
- `CameraUp`, `CameraDown`, `CameraLeft`, `CameraRight`
- `MousePrimary`, `MouseSecondary`

Bindings can target keyboard keys, mouse buttons, gamepad buttons, or gamepad axes.

## Press versus Down

`down()` means a control is currently held.

`pressed()` means a keyboard, mouse button, or gamepad button transitioned from released to pressed during the current frame. Analog-axis edge detection remains intentionally separate from button press semantics.

## Profiles

Built-in presets are available through:

- `makeDefaultInputProfile()`
- `makeKeyboardMouseProfile()`
- `makeModernGamepadProfile()`
- `makeArcadeProfile()`

A profile contains:

- profile name and schema version
- action bindings
- mouse sensitivity
- mouse Y inversion
- gamepad dead zone
- trigger threshold
- gamepad sensitivity
- selected gamepad index

## Persistence

Profiles use a small deterministic line-oriented format rather than a general JSON parser. This keeps the NEX8 runtime dependency-free and makes project configuration easy to diff in Git.

Example:

```text
# NEX8 input profile v1
schema=1
name=NEX8 Default
mouse_sensitivity=1
a... 
```

Use `serializeInputProfile()`, `loadInputProfileText()`, `saveInputProfile()`, and `loadInputProfile()` for persistence.

## Project integration

The project manifest reader remains intentionally small and currently handles the core cartridge fields. Input profiles are separate configuration artifacts so input bindings can evolve without coupling the cartridge manifest to device-specific settings.

The intended Studio layout is:

```text
Studio
└── Controls
    ├── Profile
    ├── Device
    ├── Action
    ├── Primary Binding
    ├── Secondary Binding
    ├── Dead Zone
    ├── Sensitivity
    └── Reset to Defaults
```

## Next implementation stage

The next layer should add conflict detection, binding replacement/removal APIs, human-readable control names, controller hot-plug events, and a Studio input-mapping model. Native CELL8 controller registers can then consume the same action state without coupling game logic to desktop devices.
