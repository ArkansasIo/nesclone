# C++23, NESLogic and 6502 assembly bridge

These are three separate execution targets that work together:

1. **C++23 emulator core** runs on the host PC and models NES hardware.
2. **NESLogic** runs on the host PC for game tooling, scripted test cases, and optional high-level game data logic.
3. **6502 assembly** is assembled into PRG-ROM and executes on the emulated NES CPU.

A NES cartridge does not run native x86-64/ARM C++23. C++23 game code must be compiled for the host environment, or translated by a restricted cross-compiler to 6502 machine code. The initial SDK will use ca65/ld65-compatible sources; native NESLogic-to-6502 compilation is an ambitious later milestone.

## Native host API sketch

```cpp
runtime.registerNative("controller_down", [&](const auto& args) {
    // Validate the button argument, then read the emulator's controller state.
    // Expose safe operations only; do not hand scripts raw pointers.
    return neslogic::Value(false);
});
```

## Assembly toolchain path

```text
main.s + include files + CHR assets
               |
              ca65
               |
          object files
               |
             ld65
               |
          PRG/CHR outputs
               |
        iNES/NES 2.0 packer
               |
             game.nes
```

## Compiler roadmap

- Type checking and supported subset specification
- Expressions, control flow, functions and scoped locals
- Class/method semantics on host interpreter
- Explicit compile errors for unsupported constructs
- Optional native function calls for emulator test automation
- Restricted compiler for constant expressions, simple integer arithmetic and selected branches
- Code generation for 6502 opcodes and relocation/linking
- Cycle/cost analysis and RAM budget checks
- Differential tests against expected 6502 behavior
