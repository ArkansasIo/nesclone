# Contributing

## Principles

- Keep the emulation core deterministic and independent of window/audio APIs.
- Implement hardware behavior from documented NES/2A03/2C02 behavior and verify it with tests.
- Avoid marking a feature supported solely because a class/interface exists.
- Do not commit copyrighted commercial ROMs, BIOS data or personal save files.
- Add unit tests and update relevant documentation with each hardware behavior change.

## Workflow

1. Create a focused branch.
2. Add the smallest test that reproduces the behavior.
3. Implement the behavior without unrelated refactoring.
4. Build with CMake and run CTest.
5. Include limitations or test-ROM evidence in the pull request description.

## Code style

Use C++23, lower_snake_case for local functions/variables, PascalCase for types, explicit fixed-width integer types for emulated values, and fixed-width casts where truncation is intentional. Keep warnings enabled.
