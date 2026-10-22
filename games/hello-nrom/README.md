# hello-nrom

Starter 6502 assembly cartridge sample. Copy and adapt `sdk/examples/hello/main.s` after the SDK linker/ROM-packing flow has been validated. The current version is source-only and has no checked-in commercial or prebuilt ROM image.

Build with ca65/ld65 using a verified NROM configuration; then combine the header, PRG and CHR assets with a validated cartridge packer.