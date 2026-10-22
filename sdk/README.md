# NES Homebrew SDK (starter)

The SDK contains hardware constants and assembly examples that target the 8-bit NES CPU architecture.

## Tools

- `ca65` assembler and `ld65` linker from cc65 for current assembly examples
- NESClone's future `nesrom` utility for packing a valid iNES header, PRG-ROM and CHR-ROM
- CHR, nametable, palette and disassembly tools are planned in `tools/`

## Important distinction

C++23 runs the emulator and host tools. Original NES cartridges execute 6502 machine code and contain PRG/CHR data. A standard C++23 compiler does not compile directly into NES cartridge code.

## Source layout

- `include/nes.inc`: PPU/APU/controller register constants
- `include/controller.inc`: controller button masks
- `include/macros.inc`: reusable assembly macros
- `linker/nrom128.cfg`: starter linker configuration
- `examples/hello/main.s`: NROM reset/vector example

A valid release toolchain needs integration tests that inspect the iNES header and run the emitted ROM under test hardware/emulators. The linker config is a starter and is not a complete ROM packaging pipeline by itself.
