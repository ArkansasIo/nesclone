# Game cartridges and software

A cartridge image contains a header, PRG-ROM (6502 machine code/data), CHR-ROM or CHR-RAM, mapper behavior, optional trainer and optional battery-backed RAM.

The library displays local filename, mapper/submapper, PRG/CHR sizes, mirroring and battery flag. Classic iNES does not reliably provide a verified internal display title, so the filename is only a user-friendly label.

## First-party example roadmap
- hello-nrom: reset path and vectors.
- controller-demo: serial input on $4016.
- sprite-demo: CHR tiles and OAM once PPU rendering exists.
- sound-demo: APU channels when audio synthesis is implemented.
- scrolling-demo: nametable and scroll behavior.
- logic-demo: NESLogic host logic and equivalent 6502 code.

Each game directory should contain source assembly, build config, licensed assets, README and checksums for regression ROMs. Do not commit commercial ROM images; users load files they are authorized to use.