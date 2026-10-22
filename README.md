# NESClone — 8-bit console and homebrew development platform

NESClone is a C++23 desktop-hosted 8-bit console emulator and development environment targeting the original NES cartridge architecture. The project includes an emulator core scaffold, 6502 assembly SDK, cartridge metadata tooling, XMB-style dashboard model, local ROM library/settings APIs, and an early Lua-inspired NESLogic scripting prototype.

**Status: active scaffold, not yet a complete playable or cycle-accurate emulator.** The CPU currently implements only a subset of opcodes; PPU rendering and APU synthesis are incomplete; the XMB layer is a navigation model and theme rather than a finished graphical frontend; NESLogic has a lexer/value/runtime foundation but its parser rejects executable programs.

## Project goals

- C++23 emulator core with 6502-compatible CPU, bus, PPU, APU, controllers and cartridge mappers.
- XMB-inspired console dashboard with Games, Cartridges, Controllers, Tools, Settings and System sections.
- Cartridge metadata parsing and local ROM library.
- 6502 assembly SDK and homebrew examples using ca65/ld65 during the initial stage.
- `nesrom` metadata utility and future ROM builder/CHR/nametable/palette tools.
- NESLogic: Lua-inspired control flow, functions, objects/classes, tables and host API bindings.
- Debugger/disassembler, save states, OS packaging and safe update/patch process.
- Unit tests and cross-platform CI.

## Build requirements

- CMake 3.25+
- C++23 compiler: GCC 13+, Clang 16+ or Visual Studio 2022 17.8+
- cc65 (`ca65` / `ld65`) for current assembly examples

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Commands

```sh
./build/nesclone path/to/rom.nes
./build/nesrom info path/to/rom.nes
./build/neslogic path/to/script.nsl
```

On Windows, executable paths are under the chosen build configuration directory. `nesrom info` is the only ROM tool command implemented so far. The NESLogic interpreter currently reports a clear error for non-empty scripts instead of silently claiming to run them.

## Architecture and docs

- `include/nes/`: emulator and frontend APIs
- `src/`: CPU/bus/cartridge/PPU/APU/frontend/tools implementations
- `include/neslogic/`, `src/neslogic/`: scripting language foundation
- `sdk/`: assembly includes, linker configs and examples
- `tools/romtool/`: ROM metadata CLI
- `ui/xmb/`: dashboard design notes and theme tokens
- `tests/`: core and frontend tests
- `docs/CONSOLE_PRODUCT.md`: 8-bit console product scope
- `docs/DEVELOPMENT_STATUS.md`: honest implementation status
- `docs/ROADMAP.md`: milestones and acceptance gates
- `docs/UPDATES_AND_PATCHES.md`: safe software update design
- `docs/PACKAGING.md`: platform packaging plan

## Hardware / language boundary

A physical NES executes 6502 machine code from PRG-ROM, uses CHR data for graphics, and relies on mapper hardware. C++23 and NESLogic run on the host computer and power emulator/tooling; they are not native NES cartridge binaries. A physical console clone would require separate hardware schematics and PCB validation.

## Legal

NES and Nintendo are trademarks of their respective owner. This independent project is not affiliated with Nintendo. No commercial ROMs are included. Use only ROMs and assets you have the rights to use.

## CI status

GitHub Actions builds and tests Windows, macOS and Linux. See the [Actions page](https://github.com/ArkansasIo/nesclone/actions) for the actual latest result. Do not assume a run is passing while it is queued, in progress or failed.
