# CPU: Ricoh 2A03 / 6502

The NES CPU is based on the 6502. The 2A03 disables the original decimal arithmetic behavior even though the decimal flag can still be set/cleared by instructions. The core must model the flag and interrupt stack behavior accurately.

## Registers

- A: accumulator
- X/Y: index registers
- PC: 16-bit program counter
- SP: 8-bit stack pointer, mapped to page $0100
- P: status flags C, Z, I, D, B, unused bit, V and N

## Reset and interrupt vectors

- NMI: $FFFA-$FFFB
- Reset: $FFFC-$FFFD
- IRQ/BRK: $FFFE-$FFFF

## Instruction implementation requirements

The current starter implements a subset of official opcodes for scaffolding. A complete core must add every official opcode, all addressing modes, correct cycle penalties for page crossings and branches, RMW dummy writes, interrupt polling behavior, DMA stalls, and optionally documented/unofficial opcode behavior for compatibility.

Do not treat the starter CPU as a validated 6502 implementation. Run established open test ROMs and compare instruction traces before claiming compatibility.
