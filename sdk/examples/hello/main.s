; Minimal NROM hello-loop skeleton. Assemble with ca65.
.include "../../include/nes.inc"

.segment "HEADER"
.byte $4E, $45, $53, $1A
.byte 1             ; 16 KiB PRG
.byte 1             ; 8 KiB CHR (append a CHR file with ROM builder)
.byte $00, $00      ; Mapper 0, horizontal mirroring
.byte $00, $00, $00, $00, $00, $00, $00, $00

.segment "CODE"
.proc Reset
    sei
    cld
    ldx #$40
    stx APU_FRAME
    ldx #$FF
    txs
    inx
    stx PPUCTRL
    stx PPUMASK
@wait1:
    bit PPUSTATUS
    bpl @wait1
@wait2:
    bit PPUSTATUS
    bpl @wait2
    lda #$00
    sta PPUSCROLL
    sta PPUSCROLL
    lda #%10000000
    sta PPUCTRL
    lda #%00011110
    sta PPUMASK
@main:
    jmp @main
.endproc

.proc Nmi
    rti
.endproc

.proc Irq
    rti
.endproc

.segment "VECTORS"
.word Nmi
.word Reset
.word Irq
