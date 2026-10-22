# nex8asm — CELL8 Assembler

This tool is the planned assembler for NEX8 CELL8-PPE source. The current CELL8 Pong `.asm` example shows proposed syntax and is **not yet accepted by ca65** or compiled by a complete NEX8 assembler.

## Required assembler pipeline

1. Parse directives (`.org`, labels, byte/word data and include directives).
2. Tokenize mnemonics and operands.
3. Resolve instruction encodings from `docs/CELL8_ISA.md`.
4. Perform two-pass label resolution for absolute targets.
5. Emit code bytes and symbol table.
6. Produce diagnostics with file/line/column.
7. Add golden tests comparing assembler bytes with the CELL8 CPU's opcode decoder.

Do not emit a final N8CR file until the assembler, linker layout and entry-point/section rules are stable.
