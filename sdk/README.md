# NES Homebrew SDK (starter)

This folder contains ca65 syntax hardware constants, macros, a NROM example and linker configuration. The SDK is not yet a standalone compiler or ROM packager.

## Prerequisites

Install cc65 (provides `ca65` and `ld65`) and verify the tools are on PATH.

## Current source example

`examples/hello/main.s` illustrates reset setup and vectors. The current `nrom128.cfg` config produces a PRG object layout; a future ROM packager must combine the 16-byte header, PRG image and 8 KiB CHR image in the correct iNES order. Keep those steps explicit rather than assuming linker output alone is a complete ROM.
