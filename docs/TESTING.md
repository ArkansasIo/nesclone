# Testing strategy

## Unit tests
- iNES magic, truncation, trainer offset, PRG/CHR declared lengths, mapper extraction and mirroring.
- NROM 16 KiB mirroring, 32 KiB map, CHR ROM read-only, CHR RAM writable.
- CPU registers, reset vector, flags, stack and per-opcode results.
- Controller strobe and eight-bit serial order.
- PPU register latches, mirroring, vblank/NMI, scrolling, OAM and renderer pixels.
- APU timers, envelopes, length/linear counters, frame IRQ and mixer.
- XMB navigation boundaries and persistent settings behavior.
- NESLogic lexical diagnostics and each grammar/execution feature.

## Integration / conformance
Run legally obtained public CPU/PPU test ROMs and save instruction traces. Record test ROM name/version, checksum, pass code and known incompatibility. Keep copyrighted commercial ROMs out of version control.

## Acceptance gates
1. C++ core tests pass on all CI operating systems.
2. CPU tests pass for all documented official opcodes and required timing.
3. PPU tests validate render pipeline timing and frame output.
4. APU tests validate deterministic sample output.
5. Mapper tests pass independently and with integration ROMs.
6. GUI passes input/navigation and ROM-load smoke tests.
