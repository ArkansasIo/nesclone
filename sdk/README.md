# NES homebrew SDK

This starter SDK targets the 8-bit NES CPU with ca65/ld65-compatible assembly. C++23 runs the host emulator and tools; it does not compile directly to cartridge machine code.

Tools:
- ca65 and ld65 from cc65 for assembly.
- `nesrom info <file.nes>` to inspect cartridge metadata.
- Planned native nesasm, ROM packer, CHR converter, nametable/palette tools.

Layout:
- include/nes.inc: PPU/APU/controller register constants.
- include/controller.inc: button masks.
- include/macros.inc: assembly macros.
- linker/nrom128.cfg: starter linker layout.
- examples/hello/main.s: reset/vector source.

The linker configuration is not a complete ROM packer. A finished toolchain must join the 16-byte header, PRG and CHR in correct iNES order and test the resulting ROM.