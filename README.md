# NESClone

A C++23 Nintendo Entertainment System (NES) emulator and homebrew development toolkit.

> Status: initial scaffold. The current milestone provides a portable core foundation (CPU bus, basic 6502 execution, iNES 1.0 parsing, NROM cartridge mapping, controller serial input, and a headless runner). It is not yet a cycle-accurate NES implementation. PPU rendering, APU synthesis, advanced mappers, debugger UI, assembler, save states, and comprehensive conformance testing are planned follow-up milestones.

## Features planned

- Ricoh 2A03 / 6502 CPU core, including official opcodes and documented timing
- CPU memory map and mirrored internal RAM / PPU registers
- PPU 2C02 background/sprite rendering, scrolling, NMI and OAM DMA
- APU pulse, triangle, noise and DMC audio channels
- iNES 1.0 and NES 2.0 cartridge parsing
- Mapper framework: NROM (0), MMC1 (1), UxROM (2), CNROM (3), MMC3 (4), then additional boards
- Controller serial ports ($4016/$4017)
- Desktop frontend, configurable input, save states and debugger views
- Assembly SDK, ROM builder, CHR tools, disassembler and test ROM harness
- Automated tests and cross-platform CI

## Requirements

- CMake 3.25+
- A C++23 compiler (MSVC 2022 17.8+, GCC 13+, or Clang 16+)
- Optional: SDL2 for a graphical/audio frontend in a later milestone

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
```

## Run

```sh
./build/nesclone path/to/rom.nes
```

The initial runner validates/loads a ROM and reports cartridge metadata. Gameplay compatibility depends on implementation coverage; use licensed homebrew or your own test ROMs.

## Repository layout

- `include/nes/`: public emulator interfaces
- `src/`: emulator implementation and CLI
- `tests/`: unit and integration tests
- `sdk/`: assembly examples and hardware definitions
- `docs/`: architecture and hardware references
- `.github/workflows/`: build/test CI

## Legal

NES and Nintendo are trademarks of their respective owner. This project is an independent, clean-room hobby emulator and is not affiliated with Nintendo. Do not distribute copyrighted commercial ROMs or BIOS data. Supply ROMs you are legally entitled to use.

## Roadmap

See [docs/ROADMAP.md](docs/ROADMAP.md) for implementation milestones and validation strategy.