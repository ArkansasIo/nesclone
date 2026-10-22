# NESLogic scripting language

NESClone's optional scripting/tooling language is named **NESLogic**. It is a small Lua-inspired language for game logic, emulator automation, test scripts, and build-time tools. It is not Lua-compatible and does not replace the 6502 machine code executed by an NES ROM.

## Design goals

- Lua-style control flow: `if / elseif / else / end`, `while / do / end`, `for / do / end`, and `repeat / until`
- Functions, local variables, lexical scopes, return values, strings, numbers, booleans and nil
- Object-oriented programming using class declarations, constructors, methods, fields and method dispatch
- C++23 host runtime with a small, explicit native-binding API
- Optional 6502 assembly generation for supported low-level constructs, separate from normal host-side scripts
- Friendly parse errors with source location, statement-oriented syntax and comments
- No arbitrary file/network/system access from scripts by default

## Example

```lua
class Player
    function init(self, x, y)
        self.x = x
        self.y = y
        self.health = 100
    end

    function move(self, dx, dy)
        self.x = self.x + dx
        self.y = self.y + dy
    end

    function isAlive(self)
        return self.health > 0
    end
end

local hero = Player(24, 40)

if hero:isAlive() then
    hero:move(1, 0)
elseif hero.health == 0 then
    print("player defeated")
else
    print("unknown player state")
end
```

## Grammar (initial design)

```ebnf
program       = { statement } ;
statement     = local_decl | function_decl | class_decl | if_stmt
              | while_stmt | repeat_stmt | for_stmt | return_stmt
              | assignment | expression_stmt ;
local_decl    = "local" identifier [ "=" expression ] ;
function_decl = "function" identifier "(" [ parameters ] ")" block "end" ;
class_decl    = "class" identifier [ "extends" identifier ] block "end" ;
if_stmt       = "if" expression "then" block
                { "elseif" expression "then" block }
                [ "else" block ] "end" ;
while_stmt    = "while" expression "do" block "end" ;
repeat_stmt   = "repeat" block "until" expression ;
for_stmt      = "for" identifier "=" expression "," expression [ "," expression ] "do" block "end" ;
return_stmt   = "return" [ expression ] ;
block         = { statement } ;
```

The grammar is the design contract for the first parser implementation. The first interpreter milestone only includes a subset; unimplemented constructs should return precise diagnostics instead of silently changing semantics.

## Runtime values

`nil`, booleans, signed 64-bit integers, floating-point numbers, strings, arrays/tables, native functions, user functions and instances are represented by the runtime value type. Table keys must be hashable. Class inheritance and metamethod/operator overloading are later extensions.

## Relationship to NES assembly

The script runtime runs on the host computer alongside the emulator or game tooling. It is not installed on original NES hardware. A separate restricted compiler may translate a subset of constant expressions and simple functions to 6502 assembly. Graphics, timing-sensitive audio, interrupts, and the main cartridge code should remain explicit 6502 code.
