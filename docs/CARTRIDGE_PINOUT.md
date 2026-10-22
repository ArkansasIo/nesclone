# Cartridge interface: logical reference

This is a software reference only, not manufacturing-ready electrical documentation. NES cartridges carry CPU address/data/control signals and PPU address/data/control signals to mapper and memory chips. Exact physical connector layout, signal direction, edge numbering and electrical limits must be verified against trusted hardware references before designing a PCB.

## Software-visible regions
- CPU $6000-$7FFF is commonly board RAM/battery RAM when present.
- CPU $8000-$FFFF is commonly mapper-controlled PRG-ROM.
- PPU $0000-$1FFF is mapper-controlled CHR-ROM/CHR-RAM.
- Mapper registers may be written in cartridge address space and can change PRG/CHR banks, mirroring and IRQ behavior.

A mapper must expose explicit CPU and PPU read/write mapping and preserve board-specific bus conflicts, RAM protection, mirroring and IRQ behavior where applicable.