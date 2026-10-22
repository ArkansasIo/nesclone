# NEX8 — Nexus Eight

## New product identity

**NEX8** is an original 8-bit console software platform and game-development studio. The name means **Nexus Eight**: one host platform for a console runtime, cartridge library, game engine, and cartridge IDE.

The repository began as an NES-compatible emulator scaffold. The current implementation still uses the 6502 CPU model, iNES cartridge format and NROM mapper as its first compatibility target; renaming does not change those hardware facts. NEX8 is the new product identity, while legacy NES compatibility remains a target profile.

## Brand system

- Product: NEX8
- Expanded name: Nexus Eight
- Console concept: NEX8-01
- Developer studio: NEX8 Studio
- Cartridge format: NEX8 Cartridge Project (source manifest) and `.nes` output for NES compatibility builds
- SDK namespace direction: `nex8::` for new public host APIs
- Existing compatibility namespace: `nes::` retained until a planned API migration is tested

## Scope

1. NEX8 Console Shell: game library, cartridges, settings, controller setup, tools and system diagnostics.
2. NEX8 Runtime: the emulator core and host engine.
3. NEX8 Studio: project browser, code workspace, tile/palette/nametable tools, logic editor, build pipeline and debugger.
4. NEX8 Cartridge SDK: 6502 assembly examples, C++23 host tools, Lua/NESLogic scripting for validation and authoring.
5. NEX8 Cartridge Library: metadata, local ROM references, compatibility notes and original homebrew samples.

## Important compatibility boundary

C++23 and Lua are host-side development technologies. A physical NES-compatible cartridge still needs 6502 machine code, graphics data and the required cartridge/mapper hardware. NEX8 may eventually define its own hardware profile, but that requires an explicit CPU, memory map, video/audio system, controllers, cartridge electrical interface, and validated physical design. This repository is not yet a validated hardware console design.
