# Build and packaging

Build:
```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Package goals:
- Windows: portable ZIP, optional installer.
- macOS: app bundle/DMG after signing and notarization setup.
- Linux: tarball, then AppImage/Flatpak after dependencies stabilize.

Packages should include binaries/runtime libraries, NESLogic if enabled, license notices, README, quick-start and known-limitations matrix. Never bundle copyrighted ROMs.

A release must record source commit, compiler/CMake versions, checksums and CI logs. Only report CI as passing after workflow results verify it.