# NEX8 — Nexus Eight 8-bit Console and Game Studio

**NEX8** is an original 8-bit console software platform and game-development studio. It brings a console shell, cartridge library, emulator/runtime, game engine, cartridge SDK, and developer tools into one project.

The first compatibility profile is **6502 + iNES/NROM**, so this repository can reuse established NES-compatible cartridge tools while NEX8's independent hardware profile remains a future design track. The NEX8 name does not imply that a custom physical console already exists.

**Status: active software scaffold, not a complete playable or cycle-accurate emulator and not a finished IDE.** The CPU implements only a subset of opcodes; PPU rendering and APU synthesis are incomplete; the XMB-inspired dashboard is a navigation model/theme rather than a finished GUI; and NESLogic parsing/execution remains a prototype.

## Product modules

- **NEX8 Console Shell:** game library, cartridge manager, controller setup, settings, tools and diagnostics.
- **NEX8 Runtime:** C++23 emulator facade, CPU/bus/cartridge model, PPU/APU scaffolds and deterministic engine interfaces.
- **NEX8 Engine:** reusable scene/entity/component/game-object model for host-side simulation and testing.
- **NEX8 Studio IDE:** project model, source/asset workbench design, logic-rule authoring, cartridge inspector, build output and debugger integration points.
- **NEX8 Cartridge SDK:** 6502 assembly examples, linker configuration, CHR/nametable tools and build documentation.
- **NEX8 Game Logic:** Lua-style examples and NESLogic condition/action statement primitives for host-side authoring and validation.
- **NEX8 Cartridge Library:** metadata-only catalog and user-supplied local ROM references.

## Language and hardware boundary

- C++23 powers the host emulator, game engine, Studio services and tools.
- 6502 assembly is the first native cartridge-code path for the compatibility profile.
- Lua/NESLogic is an authoring/host scripting layer; it is not executed natively by a stock 6502 console.
- A finished `.nes` image needs valid 6502 machine code, graphics data and cartridge header/mapper assumptions.
- A distinct NEX8 physical console would require a defined CPU/ISA, memory map, graphics/audio systems, controller protocol, cartridge interface, schematics and electrical validation. It is not yet a validated hardware design.

## Build requirements

- CMake 3.25+
- C++23 compiler: GCC 13+, Clang 16+ or Visual Studio 2022 17.8+
- cc65 (`ca65` / `ld65`) for assembly examples

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Current commands

```sh
./build/nesclone path/to/rom.nes
./build/nesrom info path/to/rom.nes
./build/neslogic path/to/script.nsl
```

The legacy binary name and `nes::` C++ namespace are retained temporarily to preserve compatibility while the product rebrand is introduced. The executable should be renamed to `nex8` as part of a verified build/API migration, not by changing labels alone.

## Documentation

- `PRODUCT_REBRAND.md`: rename strategy and product identity
- `docs/NEX8_PRODUCT.md`: original 8-bit console concept
- `docs/GAME_ENGINE_AND_STUDIO.md`: engine and IDE architecture
- `docs/GAME_SDK.md`: assembly/C++23/Lua cartridge-development model
- `docs/NES_CARTRIDGE_CATALOG.md`: cartridge metadata schema and catalog
- `docs/DEVELOPMENT_STATUS.md`: current implementation status
- `docs/ROADMAP.md`: milestones and acceptance gates
- `docs/PACKAGING.md`: cross-platform packaging plan
- `ui/xmb/`: console dashboard layout and theme

## Legal and asset policy

NEX8 is an independent project. NES and Nintendo are trademarks of their respective owner; references to NES describe compatibility targets only. No commercial ROMs are included. Use only ROMs and assets you have the rights to use.
