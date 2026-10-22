# NEX8 CELL8 Rev-A Architecture (Proposed)

## Purpose

CELL8 is a custom 8-bit heterogeneous-console concept inspired by the organizational idea of a general CPU orchestrating specialized processors. It is a new experimental target, separate from the existing 6502/iNES compatibility runtime.

All CPU opcode encodings, timing, the GPU pipeline and the N8CR cartridge format are experimental specifications. They must be frozen and tested before a hardware or assembler implementation is treated as stable.

## Proposed host model

| Component | Initial model |
|---|---|
| CELL8-PPE | 8-bit ALU/registers, 16-bit PC and SP, status flags |
| CELL8-GPU | 256×240 indexed framebuffer, palette RAM, sprite register model |
| CELL8-SPU | Four 8-bit-style task units, each with 256 bytes local store |
| DMA | Bounds-checked bulk copy model to memory/device buffers |
| Audio | Register block placeholder; 4-channel PSG + sample channel proposed |
| Controller | Two digital controller ports proposed |
| Cartridge | N8CR header plus program, graphics, audio and data sections |
| IDE | NEX8 Studio workspace, build output, debugger and asset editor integration |

## Memory map draft

| Range | Device |
|---|---|
| `0000–1FFF` | System RAM window (8 KiB mirrored/aliased mapping must be specified) |
| `2000–27FF` | 2 KiB video RAM |
| `2800–28FF` | Palette RAM |
| `2900–29FF` | Sprite RAM |
| `2A00–2FFF` | GPU registers |
| `3000–30FF` | Audio registers |
| `3100–31FF` | Controller registers |
| `3200–32FF` | DMA registers |
| `3300–33FF` | SPU mailboxes/control |
| `4000–7FFF` | Cartridge RAM |
| `8000–FFFF` | Cartridge ROM window |

This map is an architectural proposal, not RTL or a silicon specification. The system RAM range is an 8 KiB window; the physical storage size and aliasing rule must be fixed in the bus implementation.

## CPU architecture draft

The current CPU model specifies eight 8-bit registers A, B, C, D, E, F, X and Y; 16-bit PC and SP; and carry, zero, interrupt-disable, decimal, overflow and negative flags. Version 1 uses provisional opcodes for immediate loads, absolute memory access, immediate ALU operations, branches, increments/decrements, stack basics, and halt/no-op.

Unsupported I/O, CALL/RET, interrupt entry and detailed cycle timing must produce explicit errors until the ABI is frozen. A future revision needs a complete opcode table, instruction length/cycles, addressing modes, reset/interrupt vectors, stack policy and assembler encoding tests.

## GPU

The host model includes a 256×240 8-bit index framebuffer, 256-entry palette, VRAM and 64 sprite descriptors. Target modes, priority, tile fetch, scroll, sprite limits, transparency, raster timing, DMA interaction and video signal generation are not fully implemented.

## SPU and DMA

The current SPU is a task scheduling/local-store scaffold rather than an independent executable CPU. DMA validates source and destination spans before copying. Future work must define SPU ISA, mailbox protocol, scheduling/interrupt arbitration, DMA register semantics and device-specific transfers.

## Cartridge: N8CR v1

The 24-byte header uses ASCII `N8CR`, version and flags, mapper ID, then 32-bit little-endian byte lengths for PRG, graphics, audio and data sections. The payloads follow in that order. The host parser must reject unsupported versions, truncated files and lengths that do not match total file size. Entry point is currently a project-level field pending a header revision.

This is a project-development format proposal, not a finalized hardware standard. Do not call N8CR compatible with iNES; they are different formats.

## Development languages

- CELL8 assembly: intended target language for the custom CPU after its encoding is frozen.
- C++23: host-side emulator model, IDE, tools and tests.
- Lua-style game rules: host-side data/rule authoring. Direct CPU execution requires a compiler or a VM designed for CELL8.

## Required next gates

1. Freeze ISA and binary encoding; write assembler/disassembler and golden opcode tests.
2. Specify bus and aliasing/interrupt behavior.
3. Implement all CPU instructions, ALU flags, CALL/RET, interrupts and cycle counters.
4. Define GPU rendering semantics, sprite/tile formats and frame timing.
5. Define audio and controller registers.
6. Connect CPU bus to GPU, audio, controllers, DMA and SPUs.
7. Complete N8CR packer/validator, symbols/map output and cartridge tests.
8. Build the IDE UI and integrated debugger only over stable service APIs.
9. If pursuing physical hardware, create synthesizable RTL, simulation benches, FPGA prototype, schematics and electrical review.
