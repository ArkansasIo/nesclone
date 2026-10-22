; NESClone Pong gameplay core. This file contains game state/motion only;
; PPU drawing and OAM DMA still need integration for a playable cartridge.
.setcpu "6502"

.export update_game, reset_ball
.exportzp ball_x, ball_y, ball_vx, ball_vy, left_y, right_y

.segment "ZEROPAGE"
ball_x:      .res 1
ball_y:      .res 1
ball_vx:     .res 1
ball_vy:     .res 1
left_y:      .res 1
right_y:     .res 1
left_score:  .res 1
right_score: .res 1

.segment "CODE"
update_game:
    lda ball_x
    clc
    adc ball_vx
    sta ball_x

    lda ball_y
    clc
    adc ball_vy
    sta ball_y

    ; Top wall: point downward if the ball crosses the upper bound.
    lda ball_y
    cmp #$08
    bcs @check_bottom
    lda ball_vy
    bpl @check_bottom
    jsr reverse_y

@check_bottom:
    lda ball_y
    cmp #$E8
    bcc @check_goals
    lda ball_vy
    bmi @check_goals
    jsr reverse_y

@check_goals:
    lda ball_x
    cmp #$06
    bcs @check_right
    inc right_score
    jsr reset_ball
    rts

@check_right:
    cmp #$FA
    bcc @done
    inc left_score
    jsr reset_ball

@done:
    rts

reverse_y:
    lda ball_vy
    eor #$FF
    clc
    adc #$01
    sta ball_vy
    rts

reset_ball:
    lda #128
    sta ball_x
    lda #120
    sta ball_y
    lda #2
    sta ball_vx
    lda #1
    sta ball_vy
    lda #104
    sta left_y
    sta right_y
    lda #0
    sta left_score
    sta right_score
    rts
