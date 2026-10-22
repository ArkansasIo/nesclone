# NES Cartridge Catalog and Game Conversion Framework

This document converts the idea of an A–Z cartridge catalog into a **first-party NES homebrew catalog** for NESClone. It uses NES-specific metadata and keeps publisher/title metadata separate from ROM binaries.

## Catalog data model

Each entry should capture:

- `id`: stable lowercase identifier
- `title`: display title
- `publisher`: original publisher or `Homebrew`
- `year`: original release year, or `null` when unknown
- `mapper`: NES mapper number
- `prg_rom_bytes`: PRG-ROM byte count
- `chr_rom_bytes`: CHR-ROM byte count; zero may indicate CHR-RAM
- `prg_ram_bytes`: declared PRG RAM where known
- `region`: NTSC, PAL, or unknown
- `genre`: normalized genre
- `compatibility`: hardware and mapper notes
- `rom_filename`: local filename supplied by the user, never assumed to exist
- `rights`: provenance / usage note

Unknown fields must remain `null` or `unknown`; don't infer a cartridge's exact mapper or ROM size from its title.

## NES cartridge library

Example metadata records for original SDK samples:

| ID | Title | Type | Mapper | PRG | CHR | Notes |
|---|---|---|---:|---:|---:|---|
| `hello-nrom` | NESClone Hello NROM | SDK demo | 0 | build-defined | build-defined | First-party sample |
| `controller-demo` | NESClone Controller Demo | SDK demo | 0 | build-defined | build-defined | Reads controller input |
| `sprite-demo` | NESClone Sprite Demo | SDK demo | 0 | build-defined | build-defined | Sprite/CHR practice |
| `pong` | NESClone Pong | Homebrew | 0 | 16/32 KiB target | 8 KiB target | First-party example, requires SDK build pipeline |

This sample table is a project catalog, **not** a verified list of commercial cartridge dumps. A future importer can ingest an external catalog only when it provides reliable source citations and per-entry metadata.

## Sample catalog JSON

The machine-readable schema is stored in `data/cartridges/catalog.schema.json`; the project's starter records are in `data/cartridges/catalog.json`.

## Cartridge layout

An iNES image has a 16-byte header followed by PRG data and optional CHR data. Mapper information is encoded in the header. For the initial NROM target, common configurations are 16 KiB or 32 KiB PRG-ROM and 8 KiB CHR-ROM (or CHR-RAM). Other mappers may bank-switch memory and require mapper-specific code.

## Language layers

- **6502 assembly** is assembled and linked into PRG-ROM.
- **C++23** supplies host-side catalog, editor, tests, tools and game simulation APIs.
- **Lua/NESLogic** describes game entities and rules for host-side interpretation, validation or future compilation. The current interpreter is a prototype and does not yet translate arbitrary scripts into 6502 code.

## Rights and ROM handling

The catalog accepts local ROM filenames as references. Do not commit copyrighted commercial ROMs, BIOS files, proprietary assets, or unofficial dumps. A metadata entry does not grant permission to use or distribute the referenced ROM.
