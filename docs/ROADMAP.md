# Development roadmap

## Milestone 0 — Repository foundation
- [x] CMake C++23 project
- [x] Public subsystem interfaces
- [x] iNES parser starter
- [x] NROM mapping starter
- [x] Controller shift-register starter
- [x] CPU execution starter
- [x] CLI ROM metadata reader
- [x] initial unit tests and documentation

## Milestone 1 — Stabilize core
- [ ] Build on Windows, Linux and macOS in CI
- [ ] Fix all compiler warnings and run sanitizers
- [ ] Complete official 6502 opcode/addressing table
- [ ] Verify cycle counts, branches and page crossings
- [ ] Add CPU test ROM harness and trace output
- [ ] Improve NES 2.0 parser validation and mapper checks

## Milestone 2 — PPU
- [ ] Full CPU-visible register semantics and open bus
- [ ] VRAM/nametable/palette/OAM mirroring
- [ ] scanline/dot pipeline and framebuffer
- [ ] background and sprite fetch/render paths
- [ ] vblank/NMI, sprite-zero, overflow and scrolling
- [ ] OAM DMA timing and PPU conformance tests

## Milestone 3 — APU and timing
- [ ] pulse, triangle, noise and DMC
- [ ] frame counter and IRQ behavior
- [ ] nonlinear mixer and audio resampler
- [ ] shared CPU/PPU/APU timing scheduler

## Milestone 4 — Cartridge compatibility
- [ ] MMC1, UxROM, CNROM and MMC3
- [ ] AxROM, MMC2/MMC4 and selected MMC5 features
- [ ] battery-backed PRG RAM persistence
- [ ] board-specific nametable and IRQ handling

## Milestone 5 — User experience
- [ ] SDL2 window and audio output
- [ ] keyboard/gamepad remapping
- [ ] pause/reset/frame advance
- [ ] save/load state and screenshots
- [ ] pattern/name/palette/OAM inspector
- [ ] debugger breakpoints/disassembler/trace

## Milestone 6 — SDK and quality
- [ ] assembly examples and linker configs
- [ ] native assembler/linker and disassembler
- [ ] ROM/CHR/NAM/palette tools
- [ ] commercial compatibility only after conformance tests
- [ ] release packaging and user documentation

## Acceptance rule

A feature is not marked complete merely because its interface exists. Each subsystem must have executable tests and documented known limitations.
