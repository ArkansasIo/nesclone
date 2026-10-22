# NESClone Pong Cartridge

Original homebrew-style Pong starter project for NESClone.

## Layers

- `asm/` contains the 6502 cartridge-side implementation.
- `cpp/` contains the C++23 host-side object model used by tools, tests, and editors.
- `lua/` contains the high-level gameplay specification used by the NESLogic scripting layer.
- `assets/` contains source cartridge graphics assets.

C++23 and Lua are **host-side development layers**. A real NES cartridge executes 6502 machine code from PRG ROM and graphics data from CHR ROM/RAM. The cartridge build pipeline must therefore compile the assembly layer and package its binary output rather than attempting to execute C++23 or Lua on the console.

## Build prerequisites

Install the cc65 toolchain (`ca65` and `ld65`) before producing a `.nes` image. The current repository contains the source layout and manifest; ROM packing is intentionally kept separate from the host development layers until the cartridge linker configuration is finalized.

## Game rules

- Player 1 uses Up/Down.
- Player 2 can be driven by a second controller or a future CPU-AI routine.
- The ball reflects from the top and bottom playfield.
- A paddle collision reverses horizontal velocity.
- Crossing a goal boundary increments the opposing player's score.
