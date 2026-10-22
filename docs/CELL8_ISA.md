# CELL8-PPE Instruction Set (Provisional)

This opcode table is the initial software-model ISA for NEX8 CELL8 Rev-A. It is **not a frozen hardware specification**. Treat unknown opcodes as errors. The assembler, disassembler and unit tests must agree on each encoding before hardware RTL is considered.

## Registers

- `A, B, C, D, E, F, X, Y`: 8-bit registers
- `PC`: 16-bit program counter
- `SP`: 16-bit stack pointer
- `FLAGS`: carry, zero, interrupt-disable, decimal, overflow, negative

## Encoding conventions

- Opcodes are one byte.
- `imm8` is an 8-bit immediate byte.
- `addr16` is a little-endian 16-bit address.
- Branch instructions in ISA v0.1 use absolute 16-bit targets for simple first-pass decoding.
- Unless stated, cycle counts in this table are planned model costs, not validated silicon timings.

| Opcode | Mnemonic | Encoding length | Operands | Description |
|---:|---|---:|---|---|
| 00 | NOP | 1 | — | No operation |
| 01 | HALT | 1 | — | Halt model execution |
| 10–17 | LDI A/B/C/D/E/F/X/Y | 2 | imm8 | Load immediate into register |
| 20 | LDA | 3 | addr16 | Load A from absolute memory |
| 21 | STA | 3 | addr16 | Store A to absolute memory |
| 22 | LDB | 3 | addr16 | Load B from absolute memory |
| 23 | STB | 3 | addr16 | Store B to absolute memory |
| 30 | ADD A, imm8 | 2 | imm8 | Add immediate to A |
| 31 | SUB A, imm8 | 2 | imm8 | Subtract immediate from A |
| 32 | AND A, imm8 | 2 | imm8 | Bitwise AND |
| 33 | OR A, imm8 | 2 | imm8 | Bitwise OR |
| 34 | XOR A, imm8 | 2 | imm8 | Bitwise XOR |
| 35 | CMP A, imm8 | 2 | imm8 | Compare A with immediate |
| 38 | INC A | 1 | — | Increment A |
| 39 | DEC A | 1 | — | Decrement A |
| 3A | INC X | 1 | — | Increment X |
| 3B | DEC X | 1 | — | Decrement X |
| 40 | JMP | 3 | addr16 | Absolute jump |
| 41 | JZ | 3 | addr16 | Jump if zero |
| 42 | JNZ | 3 | addr16 | Jump if not zero |
| 43 | JC | 3 | addr16 | Jump if carry |
| 44 | JNC | 3 | addr16 | Jump if no carry |
| 48 | CALL | reserved | addr16 | Not executable in initial model |
| 49 | RET | reserved | — | Not executable in initial model |
| 4A | PUSH A | 1 | — | Stack push policy pending |
| 4B | POP A | 1 | — | Stack push policy pending |
| 50 | IN A | reserved | port | Device API pending |
| 51 | OUT A | reserved | port | Device API pending |
| 52 | WAIT | reserved | mask | Interrupt/timer semantics pending |

## Status and limitations

The first C++ interpreter implements NOP/HALT, immediate register loads, absolute A/B memory access, immediate arithmetic/logic/compare, A/X increment/decrement, absolute jumps/conditional jumps, and provisional A push/pop. I/O, WAIT, CALL/RET, interrupt dispatch, bus timing and the complete assembler are not complete. Treat this as ISA prototype 0.1.
