# NES assembly SDK

This repository plans to provide an assembler-independent homebrew SDK with ca65-compatible examples and a native assembler as a separate milestone.

## Standard registers

```asm
PPUCTRL   = $2000
PPUMASK   = $2001
PPUSTATUS = $2002
OAMADDR   = $2003
OAMDATA   = $2004
PPUSCROLL = $2005
PPUADDR   = $2006
PPUDATA   = $2007
APUSTATUS = $4015
JOY1      = $4016
JOY2      = $4017
```

## Minimal NROM example (ca65 syntax)

See `sdk/examples/hello/main.s`. Build it with cc65's `ca65` and `ld65` tools using the supplied linker configuration. The assembly example is source code; the emulator's CPU/PPU limitations still apply.

## Toolchain roadmap

- lexer/parser and expression evaluator
- 6502 opcode encoder with addressing-mode checks
- symbol table and forward reference resolution
- includes, binary includes, segments and definitions
- object format, relocation and linker
- iNES/NES 2.0 ROM writer
- CHR, nametable and palette conversion utilities
