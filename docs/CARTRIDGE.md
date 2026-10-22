# Cartridge images

## iNES 1.0

A classic iNES image starts with a 16-byte header:

- bytes 0-3: `NES\x1A`
- byte 4: PRG-ROM in 16 KiB units
- byte 5: CHR-ROM in 8 KiB units
- byte 6: mirroring, battery, trainer, lower mapper nibble
- byte 7: upper mapper nibble and format markers
- following fields: PRG RAM size and padding for the legacy format

A 512-byte trainer, when flagged, appears immediately after the header and before PRG-ROM.

## NES 2.0

NES 2.0 extends mapper/submapper fields and ROM/RAM size encoding. The parser recognizes the format marker and extended mapper/submapper fields. Full validation of all NES 2.0 RAM size encodings and every edge case remains follow-up work.

## NROM

NROM mapper 0 has no bank switching. Typical images use 16 KiB or 32 KiB PRG and either 8 KiB CHR-ROM or CHR-RAM. 16 KiB PRG is mirrored into $C000-$FFFF. PRG writes are ignored by the board. CHR-ROM is read-only; CHR-RAM is writable.

This project only accepts NROM image sizes supported by the current mapper implementation. It does not claim support for all iNES mapper IDs.
