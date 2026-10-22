# nesdis — planned 6502 disassembler

Planned interface:
```text
nesdis --prg game.nes --address 0x8000 --count 128
nesdis --trace trace.log
```

The current public helper `disassemble6502(address, bytes)` emits a DB fallback rather than a full mnemonic decode. A complete version requires a 256-opcode table, length/addressing metadata, unofficial-opcode policy, branch target calculation, and vectors/bank mapping awareness.
