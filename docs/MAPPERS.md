# Cartridge mapper roadmap

Mappers decode cartridge-space CPU writes and map PRG/CHR banks. Some also provide nametable wiring, IRQs and expansion audio. Each board should be covered by unit tests.

| Mapper | Common name | Required behavior |
|---:|---|---|
| 0 | NROM | fixed PRG/CHR mapping |
| 1 | MMC1 | serial shift register, PRG/CHR modes, mirroring, RAM enable |
| 2 | UxROM | switchable lower PRG bank, fixed final bank |
| 3 | CNROM | CHR bank switching |
| 4 | MMC3 | PRG/CHR bank modes, mirroring, IRQ counter/reload |
| 7 | AxROM | PRG bank and single-screen mirroring |
| 9 | MMC2 | latch-triggered CHR banks |
| 10 | MMC4 | latch-triggered CHR banks |
| 5 | MMC5 | extended mapping, split-screen features, expansion audio |

Each mapper must model board-specific address decode, bank bounds, RAM protection and reset state. IRQ-capable boards need to be clocked with the PPU/A12 behavior required by the hardware.

Only mapper 0 is wired into the current NES facade. Other mapper rows are a roadmap, not implemented functionality.
