# Object-oriented programming model

NESLogic uses class declarations and instance fields inspired by Lua tables and class systems common to game scripting languages.

## Objects

- A class stores its name, optional base class and method map.
- An instance stores a reference to its class and a map of fields.
- `ClassName(args...)` invokes the class constructor named `init`, if present.
- `object.method(args...)` resolves the method and passes the object as its first argument.
- `object:method(args...)` is shorthand for `object.method(object, args...)`.
- Field lookup checks instance fields, then class methods, then the base-class chain.
- Assignment writes to instance fields unless a future property descriptor explicitly says otherwise.

## Example

```lua
class Entity
    function init(self, name)
        self.name = name
        self.enabled = true
    end

    function disable(self)
        self.enabled = false
    end
end

class Enemy extends Entity
    function init(self, name, hp)
        Entity.init(self, name)
        self.hp = hp
    end

    function takeDamage(self, amount)
        self.hp = self.hp - amount
        if self.hp <= 0 then
            self.enabled = false
        end
    end
end
```

## C++23 runtime model

Host objects are exposed using registered functions rather than permitting script code to access arbitrary C++ memory. Bindings should validate types and argument counts and return script errors for invalid use. Native API registrations should be explicit and testable.
