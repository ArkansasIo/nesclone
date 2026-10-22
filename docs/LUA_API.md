# Lua-Style Game Rules

Example: `cartridges/pong/lua/game.lua`.

The script defines metadata, entities and named conditions/actions as data. It is intentionally written as a Lua-style design example, not represented as fully supported runtime syntax.

## Intended rule vocabulary

- `when`: condition to evaluate
- `doAction`: named action dispatched by the host runtime
- entities: numeric positions, dimensions and velocities
- game metadata: ID, title, system and mapper

## Planned binding APIs

A future host scripting adapter can provide `input.is_pressed(button)`, `entity.get(id)`, `entity.set(id, field, value)`, `collision.overlaps(a, b)`, `audio.play(effect)`, and `scene.request(name)`.

## Current limitation

The NESLogic parser/runtime is a prototype and is not a general Lua VM. This sample is a format proposal until parser and binding support are implemented and tested. A physical NES does not run Lua; game logic that ships in the cartridge must be compiled/translated or implemented in 6502 machine code.
