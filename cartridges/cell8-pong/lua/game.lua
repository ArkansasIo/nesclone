-- NEX8 CELL8 Pong rules. Lua-style authoring schema example.
-- It describes host-side rules and does not execute directly on the CELL8 CPU
-- until a script VM/compiler is implemented.

game {
    id = "cell8-pong",
    system = "NEX8",
    hardware = "CELL8-REV-A",
    width = 256,
    height = 240,
    refresh_hz = 60
}

object "Ball" {
    x = 128, y = 120,
    width = 8, height = 8,
    vx = 2, vy = 1
}

object "Paddle" {
    x = 16, y = 104,
    width = 8, height = 32,
    speed = 2
}

rule "top-boundary" {
    if Ball.y < 8 then
        Ball.vy = abs(Ball.vy)
    end
}

rule "bottom-boundary" {
    if Ball.y > 232 then
        Ball.vy = -abs(Ball.vy)
    end
}

rule "player-movement" {
    if input.up then Paddle.y = max(16, Paddle.y - Paddle.speed) end
    if input.down then Paddle.y = min(200, Paddle.y + Paddle.speed) end
}

rule "goal-left" {
    if Ball.x < 0 then
        score.player2 = score.player2 + 1
        Ball.reset()
    end
}
