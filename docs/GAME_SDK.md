# NESClone Game SDK

## Three-layer model

NESClone deliberately separates the development environment from the executable cartridge:

1. **6502 cartridge layer** — the only layer that becomes native NES PRG code.
2. **C++23 host SDK** — object-oriented game simulation, tests, editors, asset tools, and build orchestration.
3. **Lua/NESLogic layer** — high-level rules and data descriptions interpreted by the host or translated by a future cartridge compiler.

This prevents the SDK from making the false assumption that arbitrary modern C++ or Lua can execute directly on a stock NES CPU.

## Cartridge target

The initial target is NROM mapper 0:

- 16 KiB or 32 KiB PRG-ROM
- 8 KiB CHR-ROM, or CHR-RAM in supported images
- iNES/NES 2.0-compatible packaging
- reset/NMI/IRQ vectors

Future SDK versions can add MMC1, UxROM, MMC3, and other mapper backends.

## C++23 object model

Host-side games can model entities as ordinary C++ classes and use `neslogic::Statement`, `Expression`, `IfStatement`, `Block`, and `Action` to express gameplay rules. The resulting model is useful for deterministic tests and tooling even when the final cartridge implementation is hand-written 6502 assembly.

## High-level scripting

Lua-style source is treated as a design and scripting language. The current NESLogic parser remains a prototype; unsupported constructs must fail clearly instead of silently pretending to generate a valid ROM.

## Build pipeline

```text
Lua/NESLogic + C++23
        |
        v
  host validation/tests
        |
        v
  6502 source + CHR assets
        |
        v
 ca65 / ld65 toolchain
        |
        v
  PRG + CHR + iNES header
        |
        v
      game.nes
```
