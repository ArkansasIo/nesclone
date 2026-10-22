.setcpu "6502"

.segment "CODE"

; Minimal cartridge-side Pong state machine.
; Coordinates are 8-bit screen-space values. Rendering and sprite DMA are
; deliberately isolated so this starter can be extended without changing
; the gameplay rules.

ball_x:      .byte 128
ball_y:      .byte 120
ball_vx:     .byte 2
ball_vy:     .byte 1
player_y:    .byte 104
player2_y:   .byte 104
score_left:  .byte 0
score_right: .byte 0

update_game:
    jsr update_ball
    jsr update_players
    jsr collision_system
    rts

update_ball:
    lda ball_x
    clc
    adc ball_vx
    sta ball_x
    lda ball_y
    clc
    adc ball_vy
    sta ball_y
    rts

update_players:
    ; Controller integration is supplied by the SDK controller module.
    rts

collision_system:
    lda ball_y
    cmp #$08
    bcs check_bottom
    lda #$01
    sta ball_vy

check_bottom:
    lda ball_y
    cmp #$e8
    bcc check_goals
    lda #$ff
    sta ball_vy

check_goals:
    lda ball_x
    cmp #$08
    bcs check_right_goal
    inc score_right
    jsr reset_ball
    rts

check_right_goal:
    cmp #$f8
    bcc done
    inc score_left
    jsr reset_ball

done:
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
    rts
