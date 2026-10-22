-- NESClone Pong gameplay specification.
-- Syntax is Lua-inspired input for future NESLogic support; this is not a
-- claim that the current NESLogic parser executes arbitrary Lua.

game {
    id = "pong",
    title = "NESClone Pong",
    target = "NES",
    mapper = 0
}

object "Ball" {
    x = 128,
    y = 120,
    width = 8,
    height = 8,
    vx = 2,
    vy = 1
}

object "PlayerOne" {
    x = 16,
    y = 104,
    width = 8,
    height = 32,
    speed = 2
}

object "PlayerTwo" {
    x = 232,
    y = 104,
    width = 8,
    height = 32,
    speed = 2
}

rule "top-wall" {
    if Ball.y < 8 then
        Ball.vy = abs(Ball.vy)
    end
}

rule "bottom-wall" {
    if Ball.y > 232 then
        Ball.vy = -abs(Ball.vy)
    end
}

rule "player-one-input" {
    if input.up then PlayerOne.y = max(16, PlayerOne.y - PlayerOne.speed) end
    if input.down then PlayerOne.y = min(200, PlayerOne.y + PlayerOne.speed) end
}

rule "score-left-goal" {
    if Ball.x < 6 then
        score.right = score.right + 1
        Ball.reset()
    end
}
