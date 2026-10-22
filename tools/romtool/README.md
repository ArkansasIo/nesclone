# ROM tooling

Planned CLI operations:

```text
nesrom info game.nes
nesrom validate game.nes
nesrom build --mapper 0 --prg build/game.prg --chr build/game.chr --output game.nes
nesrom extract game.nes --output build/extracted/
```

Current emulator code can read iNES/NES 2.0 metadata and load basic mapper-0 images. The separate `nesrom` command-line tool is planned; do not assume these example commands are available yet.
