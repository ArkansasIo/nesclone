# NESClone: 8-bit console software platform

NESClone is a desktop-hosted 8-bit console emulator and development environment, initially targeting the NTSC NES family: 6502-compatible CPU, PPU, APU, cartridge boards and controller ports.

This repository currently describes and builds software, not a completed physical PCB console. A physical device requires schematics, PCB layout, cartridge connector design, signal/electrical verification, power/thermal work and component selection.

## Boot path
Application start -> XMB-style dashboard -> Games -> local cartridge library -> load ROM -> emulation window -> controller input -> return to dashboard.

## Runtime boundary
- Host C++23 runs emulator, UI and utilities.
- NES cartridges execute 6502 machine code.
- NESLogic is a host-side scripting prototype for tools and game-logic experiments.
- Assembly builds PRG-ROM; CHR assets provide graphics.
- Native host C++23 binaries cannot be copied to a real NES cartridge.

## Milestones
1. Cross-platform CI/build.
2. Full 6502 opcode/addressing/cycle behavior and CPU tests.
3. PPU background/sprite rendering, NMI and DMA.
4. APU channels and audio output.
5. Mapper support with per-board tests.
6. XMB GUI renderer, game library and controller support.
7. ROM/CHR/NAM/palette tools and SDK.
8. Signed packages, compatibility matrix and safe updates.

Only mark a subsystem complete after executable tests validate it.