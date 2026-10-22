; Controller input example in ca65 syntax.
.include "../../../include/nes.inc"
.include "../../../include/controller.inc"

.segment "ZEROPAGE"
pad_state: .res 1

.segment "CODE"
.proc ReadController
    lda #$01
    sta JOY1
    lda #$00
    sta JOY1
    ldx #$08
    lda #$00
    sta pad_state
@read:
    lda JOY1
    and #$01
    beq @zero
    sec
    bcs @shift
@zero:
    clc
@shift:
    ror pad_state
    dex
    bne @read
    rts
.endproc
