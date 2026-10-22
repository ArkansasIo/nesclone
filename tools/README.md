# Host tools

Built by CMake when enabled:
- `nesrom info ROM.nes`: cartridge header and payload metadata.
- `chrtool info FILE.chr`: CHR byte/tile count and basic size validation.
- `namtool info FILE.nam`: basic nametable file size diagnostic.
- `inesutil validate ROM.nes`: validates the image using the shared cartridge loader.
- `nesdis raw FILE`: byte-oriented disassembly placeholder; the complete 6502 opcode table is not yet implemented.

The native assembler, linker, ROM packer, CHR image conversion and graphical asset editors remain future milestones.
