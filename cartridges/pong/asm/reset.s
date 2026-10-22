.setcpu "6502"

.segment "HEADER"
.byte $4e, $45, $53, $1a
.byte 2
.byte 1
.byte $00
.byte $00
.byte $00, $00, $00, $00, $00, $00, $00, $00

.segment "CODE"

reset:
    sei
    cld
    ldx #$40
    stx $4017
    ldx #$ff
    txs
    inx
    stx $2000
    stx $2001
    stx $4010

wait_vblank:
    bit $2002
    bpl wait_vblank

main_loop:
    jsr update_game
    jsr wait_frame
    jmp main_loop

wait_frame:
    bit $2002
    bpl wait_frame
    rts

.include "game.s"

.segment "VECTORS"
.word reset
.word reset
.word reset
