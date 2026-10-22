# PPU: 2C02

The PPU produces a 256x240 pixel image from nametables, attribute tables, pattern tables, palette RAM and sprite OAM. A compatible implementation needs a scanline/dot renderer rather than rendering only once per frame.

## CPU-visible registers

| Address | Register | Purpose |
|---|---|---|
| $2000 | PPUCTRL | nametable, increment, sprite/background pattern bases, sprite size, NMI |
| $2001 | PPUMASK | grayscale, left-edge clipping, rendering enables, emphasis |
| $2002 | PPUSTATUS | vblank, sprite-zero hit, sprite overflow |
| $2003 | OAMADDR | OAM address |
| $2004 | OAMDATA | OAM read/write |
| $2005 | PPUSCROLL | coarse/fine scroll latch |
| $2006 | PPUADDR | VRAM address latch |
| $2007 | PPUDATA | buffered VRAM data port |

Registers $2008-$3FFF mirror $2000-$2007.

## Internal state

A full implementation needs v/t VRAM address latches, fine-X, write toggle, data read buffer, loopy scrolling increments/copies, open bus, scanline/dot counters, odd-frame behavior and correct vblank set/clear timing.

## Rendering

1. Fetch nametable tile index.
2. Fetch attribute byte and select the two-bit palette.
3. Fetch low/high bitplanes from the selected pattern table.
4. Load and shift background shifters on the proper dots.
5. Evaluate and fetch sprite rows with 8x8 or 8x16 addressing, flips, priority and sprite-zero identity.
6. Resolve background/sprite transparency and priority into the pixel palette.
7. Enforce sprite overflow and sprite-zero-hit behavior.

The current milestone includes only a placeholder CPU register byte array in the bus. It does not render graphics yet.
