# 6502 assembly reference

This is a compact quick reference. It is not the complete official programming manual.

## Addressing examples
```asm
LDA #$10       ; immediate
LDA $20        ; zero page
LDA $20,X      ; zero-page indexed
LDA $2000      ; absolute
LDA $2000,X    ; absolute indexed
LDA ($20,X)    ; indexed indirect
LDA ($20),Y    ; indirect indexed
```

## Reset/vector addresses
- NMI vector: $FFFA
- Reset vector: $FFFC
- IRQ/BRK vector: $FFFE

## Hardware ports
- PPU registers: $2000-$2007, mirrored through $3FFF
- OAM DMA: $4014
- Controller port 1: $4016
- Controller port 2 / APU frame counter: $4017
- APU status: $4015

Real code must respect vblank timing, PPU address latches, status read side effects, stack behavior, and mapper registers. Consult the full CPU/PPU/APU docs and test ROMs.
