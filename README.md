# NEX8 — Nexus Eight 8-bit Console and Game Studio

**NEX8** is an original 8-bit console software platform and game-development studio. It brings a console shell, cartridge library, emulator/runtime, game engine, cartridge SDK, and developer tools into one project.

**[Open the NEX8 Game Console & Games website](https://arkansasio.github.io/nesclone/)**

The active development branch is `nex8-rebrand`. The current implementation includes the legacy 6502 + iNES/NROM compatibility path plus the experimental **CELL8 Rev-A** 8-bit architecture, native N8CR cartridge model, RAMSQL/N8DB dataset layer, controller/keyboard/mouse input stack, and build tooling. The CELL8 design is a software model and development target; it is not yet a validated physical console.

**Status: active software scaffold, not a complete playable or cycle-accurate emulator and not a finished IDE.** The CPU implements only a subset of opcodes; PPU rendering and APU synthesis are incomplete; the XMB-inspired dashboard is a navigation model/theme rather than a finished GUI; and NESLogic parsing/execution remains a prototype.

## Product modules

- **NEX8 Console Shell:** game library, cartridge manager, controller setup, settings, tools and diagnostics.
- **NEX8 Runtime:** C++23 emulator facade, CPU/bus/cartridge model, PPU/APU scaffolds and deterministic engine interfaces.
- **NEX8 Engine:** reusable scene/entity/component/game-object model for host-side simulation and testing.
- **NEX8 Studio IDE:** project model, source/asset workbench design, logic-rule authoring, cartridge inspector, build output and debugger integration points.
- **NEX8 Cartridge SDK:** native CELL8 direction plus 6502/iNES compatibility tooling and documentation.
- **NEX8 Game Logic:** Lua-style examples and NESLogic condition/action statement primitives for host-side authoring and validation.
- **NEX8 Cartridge Library:** metadata-only catalog and user-supplied local ROM references.

## Language and hardware boundary

- C++23 powers the host emulator, game engine, Studio services and tools.
- 6502 assembly remains the legacy compatibility cartridge-code path.
- CELL8 assembly is the experimental native NEX8 cartridge-code direction.
- Lua/NESLogic is an authoring/host scripting layer; it is not executed natively by a stock 6502 console.
- Native NEX8 cartridges use the separate N8CR format; legacy `.nes` images remain iNES-compatible artifacts.
- A physical NEX8 console would require a finalized CPU/ISA, memory map, graphics/audio systems, controller protocol, cartridge interface, schematics and electrical validation.

## Build requirements

- CMake 3.25+
- C++23 compiler: GCC 13+, Clang 16+ or Visual Studio 2022 17.8+
- cc65 (`ca65` / `ld65`) for legacy 6502 assembly examples

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Windows convenience scripts are available under `scripts/`:

```bat
scripts\configure.bat
scripts\build.bat
scripts\test.bat
scripts\run_nesclone.bat
```

## Current commands

```sh
./build/nesclone path/to/rom.nes
./build/nesrom info path/to/rom.nes
./build/neslogic path/to/script.nsl
```

The legacy binary name and `nes::` C++ namespace are retained temporarily to preserve compatibility while the product rebrand is introduced.

## Documentation

- `PRODUCT_REBRAND.md`: rename strategy and product identity
- `docs/NEX8_PRODUCT.md`: original 8-bit console concept
- `docs/GAME_ENGINE_AND_STUDIO.md`: engine and IDE architecture
- `docs/GAME_SDK.md`: assembly/C++23/Lua cartridge-development model
- `docs/CELL8_ARCHITECTURE.md`: CELL8 Rev-A architecture
- `docs/CELL8_ISA.md`: provisional CELL8 instruction set
- `docs/CELL8_RAMDB_SERVICE.md`: CELL8 RAMSQL service interface
- `docs/N8DB_BINARY.md`: binary RAMSQL dataset format
- `docs/DEVELOPMENT_STATUS.md`: current implementation status
- `docs/ROADMAP.md`: milestones and acceptance gates
- `docs/PACKAGING.md`: cross-platform packaging plan
- `ui/xmb/`: console dashboard layout and theme

## GitHub Pages

The `nex8-rebrand` branch contains a static game-console website in `docs/` and a GitHub Actions deployment workflow in `.github/workflows/pages.yml`. GitHub Pages should use **GitHub Actions** as its build/deployment source.

## Legal and asset policy

NEX8 is an independent project. NES and Nintendo are trademarks of their respective owner; references to NES describe compatibility targets only. No commercial ROMs are included. Use only ROMs and assets you have the rights to use.
