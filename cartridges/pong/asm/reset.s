; NESClone Pong reset/startup. ca65 source.
; Actual ROM building requires a matching ld65 configuration and pong.chr asset.
.setcpu "6502"

.export reset, nmi, irq
.import update_game

.segment "HEADER"
    .byte $4E, $45, $53, $1A
    .byte 2                    ; 32 KiB PRG-ROM
    .byte 1                    ; 8 KiB CHR-ROM
    .byte $00, $00              ; Mapper 0, horizontal mirroring
    .byte $00, $00, $00, $00, $00, $00, $00, $00

.segment "ZEROPAGE"
frame_counter: .res 1

.segment "CODE"
reset:
    sei
    cld
    ldx #$40
    stx $4017
    ldx #$FF
    txs
    inx
    stx $2000
    stx $2001
    stx $4010

@wait_vblank:
    bit $2002
    bpl @wait_vblank

    lda #$00
    sta frame_counter

@main_loop:
    bit $2002
    bpl @main_loop
    jsr update_game
    inc frame_counter
    jmp @main_loop

nmi:
    rti

irq:
    rti

.segment "VECTORS"
    .addr nmi, reset, irq

.segment "CHR"
    .incbin "assets/pong.chr"
