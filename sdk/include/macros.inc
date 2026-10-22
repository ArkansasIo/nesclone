; Common ca65 macros
.macro WAIT_VBLANK
@wait_vblank:
    bit $2002
    bpl @wait_vblank
.endmacro

.macro SET_PPUADDR addr
    lda #>addr
    sta $2006
    lda #<addr
    sta $2006
.endmacro

.macro PUSHA
    pha
.endmacro

.macro POPA
    pla
.endmacro
