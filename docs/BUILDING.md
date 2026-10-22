# Building NESClone

## C++23 tools
Install CMake 3.25+ and a compiler with C++23 support.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Build targets include `nesclone`, `nesrom`, `inesutil`, `chrtool`, `namtool`, and `nesdis`. If scripting is enabled, CMake also builds `neslogic`.

## Assembly examples
Install cc65 to get `ca65` and `ld65`. Assembly example files are under `sdk/examples`. Current linker configuration and examples are reference scaffolding; ROM packing is not yet a complete verified end-to-end build.

## Runtime note
The current `nesclone` command loads cartridge metadata and initializes the starter CPU path, but no completed frame renderer or audio backend is integrated. It does not yet provide a playable console experience.