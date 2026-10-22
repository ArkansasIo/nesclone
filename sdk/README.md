# NEX8 SDK

The SDK contains two explicitly separate targets:

- **CELL8 / N8CR:** proposed custom NEX8 8-bit hardware profile. Its assembly syntax, ISA, and cartridge structure are experimental; the dedicated assembler and integrated emulator are not complete.
- **6502 / iNES:** compatibility target for the existing emulator scaffold. Use ca65/ld65 for assembly examples where appropriate.

## Tools

- `nesrom info <file.nes>` — inspect an iNES compatibility image.
- `cell8rom <file.n8c>` — inspect an N8CR CELL8 cartridge.
- `chrtool`, `namtool`, `inesutil`, `nesdis` — current diagnostic tools for the compatibility path.
- `nex8asm` — planned assembler for CELL8 source.
- `nex8link` — planned linker/section layout.
- `nex8pack` — planned N8CR packager.

## Language boundary

C++23 is used for host tools, engine and tests. Lua/NESLogic is an authoring/host-side scripting layer. Native cartridge software must be compiled into target machine code.

See `docs/CELL8_ARCHITECTURE.md`, `docs/CELL8_ISA.md` and `docs/CELL8_ASSEMBLY.md`.
