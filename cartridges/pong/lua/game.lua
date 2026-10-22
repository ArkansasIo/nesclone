game "Pong"

object "Ball" {
    x = 128,
    y = 120,
    vx = 2,
    vy = 1
}

object "Player" {
    x = 16,
    y = 104,
    speed = 2
}

rule "top-wall" {
    if Ball.y < 8 then
        Ball.vy = 1
    end
}

rule "bottom-wall" {
    if Ball.y > 232 then
        Ball.vy = -1
    end
}

rule "player-input" {
    if input.up then Player.y = Player.y - Player.speed end
    if input.down then Player.y = Player.y + Player.speed end
}
