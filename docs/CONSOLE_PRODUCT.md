# NESClone 8-bit game console platform

## Product definition

NESClone is a desktop-hosted 8-bit console development and emulation platform inspired by the classic 8-bit cartridge console generation. Its first compatibility target is the original NTSC NES family: 6502-compatible CPU, tile/sprite PPU, APU audio, controller ports and ROM cartridges.

The project is not a physical console design yet. A real hardware clone would additionally need schematics, PCB layout, signal/electrical validation, cartridge connector design, power/thermal analysis and licensed component choices. The current repository delivers the software platform and homebrew toolchain groundwork.

## User experience

Boot -> XMB-style home dashboard -> choose Game Library -> select a user-provided .nes cartridge -> load into emulator -> play with configurable controller -> save data and return to dashboard.

Dashboard categories: Games, Cartridges, Controllers, Tools, Settings, System. Disabled rows represent features without a working implementation and must be enabled only when their backend exists.

## 8-bit design constraints

- CPU register and address behavior uses fixed-width integers.
- ROMs contain 6502 machine code and graphics data, not host C++23 binaries.
- NESLogic scripts and the C++23 emulator execute on the host computer.
- Cartridge format and mapper support vary; initial hardware board support is NROM (mapper 0).
- Keep hardware timing and CPU/PPU/APU responsibilities separate.
- Target original 256x240 output with nearest-neighbor scaling and optional integer scaling.

## Milestone gates

1. Build/test on Windows, macOS and Linux.
2. Complete official 6502 instructions and CPU test ROM compatibility.
3. Implement PPU background and sprite rendering plus NMI/DMA timing.
4. Implement APU channel emulation and audio output.
5. Add mappers individually and test each one.
6. Add a usable XMB frontend and controller integration.
7. Finish ROM/CHR tools, SDK, save-state format and packaging.
8. Publish only with a clear compatibility matrix and ROM/legal notice.

## Games and software

The repository may include original homebrew source, generated test ROMs and public-domain assets whose licenses are recorded. Commercial games and copyrighted ROM images are not bundled. Users select local ROMs they are legally entitled to use.
