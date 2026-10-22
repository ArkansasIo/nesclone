# nesrom command line

Implemented: `nesrom info <file.nes>` prints iNES/NES 2.0 format, mapper, PRG/CHR sizes and metadata.

Planned:
```text
nesrom validate <file.nes>
nesrom build --mapper 0 --prg game.prg --chr game.chr --output game.nes
nesrom extract <file.nes> --output extracted/
```

Only `info` is currently implemented. Build/extract features need tests that validate exact iNES byte layout.