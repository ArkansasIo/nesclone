# Public C++ API

## Emulator core
- `nes::NES`: currently loads supported NROM images, resets and advances the CPU core.
- `nes::Bus`: CPU address space adapter.
- `nes::Cpu6502`: partial 6502 instruction execution.
- `nes::Mapper` and `nes::Nrom`: cartridge mapping abstractions.
- `nes::Controller`: controller state and serial shift behavior.
- `nes::PPU`, `nes::APU`: current register/latch prototypes, not full hardware models.

## Frontend API
`nes::frontend::IEmulatorService`, `ILibraryService`, `ISettingsService` separate the UI from the core. `XmbModel` provides category/menu navigation. `scanRomLibrary` searches local paths for valid .nes files. `SettingsStore` offers local quoted key/value persistence.

## NESLogic API
The language frontend contains token, lexer, parser, runtime environment and typed values. The parser currently rejects all non-empty programs intentionally; the environment/value objects are not yet a full script execution engine. The design and syntax contract is in SCRIPTING_LANGUAGE.md.

## API stability
The project is version 0.x and public APIs may change. Avoid persisting implementation pointers or assuming unsupported mapper behavior. API methods return error results rather than silently claim success where the API supports it.
