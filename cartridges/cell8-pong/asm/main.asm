; CELL8-PPE custom ISA sample for the NEX8 CELL8 Rev-A proposal.
; Opcode values and encodings are provisional and specific to the new CELL8 CPU.
; This file is sample assembly syntax, not accepted by ca65 or the legacy NES core.

.org $8000

RESET:
    SEI
    LDI A, #$00
    STA GPU_CTRL

    LDI A, #$80
    STA BG_X
    LDI A, #$00
    STA BG_Y

    LDI A, #$80
    STA BALL_X
    LDI A, #$78
    STA BALL_Y

MAIN:
    JSR READ_CONTROLLER
    JSR UPDATE_BALL
    JSR UPDATE_PADDLES
    JSR COLLISION_RULES
    JSR WAIT_VBLANK
    JMP MAIN

UPDATE_BALL:
    LDA BALL_X
    ADD A, #$02
    STA BALL_X
    LDA BALL_Y
    ADD A, BALL_VY
    STA BALL_Y
    RTS

COLLISION_RULES:
    LDA BALL_Y
    CMP A, #$08
    JNC CHECK_BOTTOM
    LDI A, #$01
    STA BALL_VY
CHECK_BOTTOM:
    LDA BALL_Y
    CMP A, #$E8
    JC CHECK_GOALS
    LDI A, #$FF
    STA BALL_VY
CHECK_GOALS:
    RTS
