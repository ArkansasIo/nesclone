# NEX8 Game Engine and Studio Architecture

NEX8 combines a host-side game engine with a development studio. This document describes how the two fit together, what is implemented today, and what remains planned.

## Module overview

- **NEX8 Engine** (`include/nes/engine/`, `src/engine/`): a deterministic, host-side scene/entity/component model used for simulation and testing. It is deliberately separated from the emulation core so a game can be authored and validated without touching the emulator.
- **NEX8 Runtime** (`include/nes/`, `src/nes/`): the emulation facade — CPU, bus, cartridge mappers, PPU and APU scaffolds, controllers and input.
- **NEX8 Studio** (designed in `docs/IDE_DESIGN.md`, layout/theme under `ui/xmb/`): the IDE workbench that ties projects, sources, assets, logic rules, cartridge inspection, build output and debugging together.
- **NEX8 Game Logic**: Lua-style examples and NESLogic condition/action statement primitives (`include/neslogic/`, `src/neslogic/`) for host-side authoring and validation.

## Engine model

The engine follows a scene → entity → component pattern:

- A **Scene** owns entities and provides update/draw ordering.
- An **Entity** is an identifier with attached components.
- **Components** carry data (transform, sprite, body, behavior, audio emitter) and are processed by systems.
- **Systems** run in a fixed order each tick, keeping simulation deterministic.

The engine targets the same fixed tick rate as the runtime (one logical frame per NTSC frame) so a game validated in the engine behaves consistently when played through the runtime facade.

## Runtime ↔ engine boundary

| Concern | Engine | Runtime |
|---|---|---|
| Input | reads a plain input state struct | fills it from controller/keyboard/mouse stack |
| Rendering | produces a draw list / framebuffer writes | PPU scaffold interprets it |
| Audio | emits notes/effects via the tracker API | APU synthesis mixes them |
| Persistence | plain structs | cartridge/N8DB layer |

The engine never includes emulator internals, and the runtime never depends on engine types. This keeps either side replaceable (e.g., a different renderer or a different CPU core).

## Studio composition

Studio is organized as the docks described in `IDE_DESIGN.md`:

- **Left:** project explorer and asset browser.
- **Center:** source editor, visual logic editor, or graphics tools.
- **Right:** properties, cartridge inspector, documentation.
- **Bottom:** build console, test results, debugger watches.
- **Preview:** 256×240 target display with frame stepping and overlays.

Build profiles select between CELL8 (N8CR cartridge), legacy 6502/iNES, and host-simulation targets. Profile and cartridge format must always agree — an iNES image is never treated as CELL8 code and vice versa.

## Game logic authoring

Games can be authored at three levels:

1. **Lua/NESLogic rules** — conditions and actions mapped onto the C++ statement model; validated on the host before packaging.
2. **C++23 engine code** — full control via the engine's scene/component API.
3. **6502/CELL8 assembly** — cartridge-native code for the legacy compatibility path and the experimental CELL8 direction.

Studio's logic editor is the intended surface for level 1; the code editor for levels 2 and 3.

## Implementation status

- Engine: scene/entity/component scaffolding exists; systems are minimal.
- Runtime: CPU is a starter opcode subset; PPU lacks a completed renderer; APU has the 9-voice synthesis and mod tracker but is not cycle-accurate.
- Studio: specification and panel definitions exist; no graphical IDE is implemented yet.
- Logic: NESLogic has lexer/value/runtime foundations; full parsing/execution is a prototype.

See `docs/DEVELOPMENT_STATUS.md` for the current status table and `docs/ROADMAP.md` for acceptance gates.
