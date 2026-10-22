# NEX8 Studio IDE Design Specification

## Workspace

- **Project explorer:** projects, cartridges, assembly, C++23 host sources, Lua/NESLogic scripts, graphics, audio, maps and tests.
- **Code editor:** syntax-aware CELL8 assembly and host language editing; line/column diagnostics and symbol navigation.
- **Graphics tools:** 8x8 tile editor, sprite editor, palette editor, nametable/map editor, sprite sheet import and indexed-image validation.
- **Logic editor:** expression/condition/action graph mapped to the C++ logic statement model and Lua-style rules.
- **Cartridge inspector:** header/version, CPU target, mapper, memory sizes, section sizes, entry point and compatibility warnings.
- **Build console:** assembler/linker/asset-tool output and build artifact list.
- **Debugger:** CPU registers, PC/SP/flags, step/run/pause/reset, memory watch, disassembly, breakpoints and call stack when supported.
- **Emulator preview:** frame-step, reset, controller input, FPS/tick counter, framebuffer and diagnostic overlay.
- **System panel:** runtime profile selector (CELL8 experiment vs legacy 6502/iNES compatibility), settings and logs.

## Main build profiles

1. **NEX8 CELL8:** planned custom CPU and N8CR cartridge profile; uses CELL8 assembly and NEX8 runtime.
2. **Legacy 6502/iNES compatibility:** existing code path for compatible images; uses ca65/ld65 where appropriate.
3. **Host Simulation:** C++23 object model and Lua/NESLogic rules for rapid host-side test runs.

The IDE must label each profile clearly. A CELL8 N8CR cartridge must never be passed to the iNES loader, and an iNES image must not be treated as native CELL8 code.

## New game workflow

1. Choose a template (Blank CELL8, CELL8 Pong, 6502 Hello, controller demo or sprite demo).
2. Create `project.json`, source folders, asset folders and tests.
3. Select CPU profile and cartridge format.
4. Validate manifest and required file paths.
5. Run host logic tests.
6. Assemble/compile the target sources.
7. Package and validate the appropriate cartridge format.
8. Launch the matching emulator profile for preview.

## UI composition

- Left dock: Project Explorer and Assets.
- Center: Source editor or visual logic/graphics editor.
- Right dock: properties, registers, cartridge metadata and documentation.
- Bottom dock: build output, test results, logs and debugger watches.
- Preview panel: 256x240 target display scaled with nearest-neighbor filtering.

## Status

This file is an IDE specification. Current implementation includes the project API, workspace panel definitions, CPU/GPU/SPU host models and ROM metadata tools. A finished graphical IDE, full assembler/linker, editor widgets, integrated debugger, and profile-aware emulator switching still require implementation.
