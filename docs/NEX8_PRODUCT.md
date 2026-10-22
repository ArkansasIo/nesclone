# NEX8 Product Concept

NEX8 (Nexus Eight) is the new identity for the 8-bit console runtime and development studio.

## Product modules

- **NEX8 Console Shell** — dashboard, game/cartridge library, controllers, settings, logs and tools.
- **NEX8 Engine** — deterministic host-side scene/entity/component model plus the emulation facade.
- **NEX8 Studio** — project creation, source editing, asset inspection, logic graph, cartridge inspector, build console and debugger.
- **NEX8 SDK** — assembly headers, linker definitions, asset tools, sample projects and tests.
- **NEX8 Cartridge Library** — machine-readable metadata; local ROM paths remain user-provided and do not imply distribution permissions.

## Hardware strategy

The first compatibility target remains the 6502/iNES/NROM ecosystem because that is what the current code models. A distinct NEX8 hardware platform is a later research track, not a claim that the current implementation is a unique physical console. Before a custom board can be implemented, specify:

- CPU/ISA and clock
- address map, RAM/ROM size and bank switching
- graphics and audio hardware
- controller protocol
- cartridge connector, voltage levels and bus timings
- boot firmware, debugging and test fixtures
- schematics, PCB constraints and electrical validation

## Software architecture

The project uses C++23 for host-side runtime, emulator, IDE services and tools. 6502 assembly is the first cartridge-native language. Lua/NESLogic is an authoring and host scripting layer. A later NEX8-native bytecode or ISA can be considered only with a defined VM or processor specification.

## Product status

This is a software product concept and early emulator/SDK scaffold. It is not yet a finished graphical IDE, cycle-accurate emulator, ROM compiler or validated physical console.
