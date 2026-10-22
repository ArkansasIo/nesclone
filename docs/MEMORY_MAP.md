# NES memory map

## CPU address space

| Range | Function |
|---|---|
| $0000-$07FF | 2 KiB internal RAM |
| $0800-$1FFF | mirrored internal RAM |
| $2000-$2007 | PPU registers |
| $2008-$3FFF | mirrored PPU registers |
| $4000-$4013 | APU channel registers |
| $4014 | OAM DMA register |
| $4015 | APU status |
| $4016 | controller 1 strobe/read |
| $4017 | controller 2 read / APU frame counter write |
| $4018-$401F | test/unused area |
| $4020-$FFFF | cartridge and mapper space |

Initial implementation note: CPU RAM mirrors are implemented. PPU registers and most APU/I/O behavior are placeholders; CPU cartridge accesses use the current mapper abstraction.

## PPU address space

- $0000-$1FFF: cartridge CHR pattern data
- $2000-$2FFF: nametables
- $3000-$3EFF: nametable mirror region
- $3F00-$3FFF: palette RAM and its mirrors

Nametable physical storage depends on cartridge mirroring / board wiring. Palette addresses have special mirrors at $3F10/$3F14/$3F18/$3F1C.
