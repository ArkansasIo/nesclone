# Architecture

NESClone is split into deterministic emulation core, host-facing frontend, tools, and validation layers.

## Dependency direction

```text
CLI / Frontend
     |
     v
    NES facade
     |
     +-- CPU6502 ---- Bus ---- Mapper / Cartridge
     |                 |
     |                 +---- PPU register adapter (placeholder in initial milestone)
     |                 +---- APU/I/O register adapter (placeholder in initial milestone)
     |                 +---- Controller serial port
     |
     +-- PPU (planned)
     +-- APU (planned)
```

The emulator core must not require a graphics framework. This keeps unit tests portable and enables future SDL2, native desktop, and headless frontends.

## Timing model

The current milestone advances CPU instructions through a simple instruction/cycle interface. It does not yet align every bus access with the PPU's 3:1 NTSC clock ratio. The production timing model should advance CPU, PPU and APU from a master clock or explicit CPU cycles, representing DMA stalls, NMI edges, APU frame sequencing, odd-frame rendering skips and mapper IRQ clocks.

## Error handling

ROM parsing returns a `LoadResult` carrying either a validated image or an error string. Invalid/truncated files must fail before the emulator uses their contents. Mappers communicate address-range ownership by returning a boolean.

## Determinism

The core should keep host input, audio device buffering and wall-clock scheduling outside the emulation state. This allows ROM tests to run headlessly and makes future save states reproducible.

## Initial milestone boundaries

Implemented: RAM mirroring, controller serial shift, iNES image parsing, basic NROM PRG/CHR mapping, a starter subset of 6502 instructions and a CLI cartridge report.

Not implemented yet: full 6502 opcode table and bus-cycle accuracy, PPU pipeline, APU audio, DMA timing, save states, advanced mapper logic, desktop frontend, native assembler and robust commercial-ROM compatibility.
