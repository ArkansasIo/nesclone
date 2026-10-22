# NEX8 — Nexus Eight Console Software Platform

NEX8 is an original 8-bit console software platform and developer studio. The current software scaffold uses a 6502-compatible CPU model and iNES/NROM as an initial compatibility profile. That compatibility target is not the same thing as a completed custom hardware console.

## Product components

- Console shell and local cartridge library
- Emulator/runtime with CPU, bus, cartridge, PPU/APU and controller modules
- Reusable C++23 scene/entity/component engine for host tools and simulation
- NEX8 Studio IDE services, workspace model and cartridge project validation
- Cartridge SDK with 6502 assembly examples and asset tool prototypes
- Lua-style/NESLogic authoring prototypes
- Debugger, disassembler, tests, releases and patch workflow

## Boot/use path

Application start -> NEX8 dashboard -> Games/Cartridges -> local library -> load a compatible image -> emulator session -> controller input -> debugger or return to dashboard.

## Hardware and language boundary

- C++23 runs on the host and powers the development environment.
- 6502 assembly is the first native cartridge path for the NES compatibility profile.
- Lua/NESLogic currently serves as host-side authoring/scaffolding; it is not cartridge machine code.
- A true NEX8 hardware console needs a separately approved hardware specification, schematic, PCB, bus timing, power design and electrical validation.

## Acceptance milestones

1. Cross-platform CI/build and packaging
2. Full CPU opcode/addressing/cycle tests
3. PPU renderer, NMI and DMA
4. APU channels and audio output
5. Mapper support with per-board validation
6. Graphical NEX8 dashboard and Studio IDE
7. Verified cartridge compiler/packer and asset editors
8. Custom hardware feasibility study, only after specification and prototype tests

A subsystem is complete only after tests validate it. A C++ interface or mockup alone is not a completed feature.
