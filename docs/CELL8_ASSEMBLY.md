# CELL8 Assembly Programming Guide (Provisional)

CELL8 assembly is the proposed assembly language for the CELL8-PPE CPU model. It uses 8-bit data registers and a 16-bit program counter. The shown source is conceptual until the dedicated assembler is complete.

## Register names

`A, B, C, D, E, F, X, Y, PC, SP, FLAGS`

## Directives

- `.org $8000`: set the source address (provisional).
- `.byte $01, $02`: emit raw bytes (planned).
- `.word $8000`: emit little-endian word (planned).
- `.include "file.inc"`: include symbols/macros (planned).

## Example

```asm
.org $8000
RESET:
    LDI A, #$00
    STA GPU_CTRL
MAIN:
    JSR UPDATE
    JMP MAIN
```

## Proposed GPU memory-mapped registers

| Address | Name | Description |
|---:|---|---|
| $2A00 | GPU_CTRL | Enable/display modes |
| $2A01 | GPU_STATUS | Frame/vblank status |
| $2A02 | BG_X | Horizontal scroll |
| $2A03 | BG_Y | Vertical scroll |
| $2A04 | SPR_X | Sprite X staging register |
| $2A05 | SPR_Y | Sprite Y staging register |
| $2A06 | SPR_TILE | Sprite tile index |
| $2A07 | SPR_ATTR | Sprite attributes |
| $2A08 | PAL_INDEX | Palette index |
| $2A09 | PAL_DATA | Palette data |

Symbol definitions should live in `sdk/cell8/include/gpu.inc` when that SDK include is added. Register and instruction semantics are provisional.

## Branches and logic

The first proposed branch instructions use absolute targets, e.g. `JZ label`, `JNZ label`, `JC label`, and `JNC label`. Arithmetic operations update the zero/negative flags; ADD/SUB/CMP also define carry and overflow rules. These rules must be checked against the ISA golden tests before being frozen.

## Calling convention

CALL/RET and interrupt entry are reserved until stack, vector, and reentrancy policies are defined. Until then, examples avoid a claim that subroutine calling is executable in the prototype CPU.
