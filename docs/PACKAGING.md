# Build and packaging

## Native build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Planned packages

- Windows: ZIP portable build and installer
- macOS: app bundle/DMG (code signing and notarization when configured)
- Linux: tarball and AppImage/Flatpak after dependency policy is fixed

## Package contents

- NESClone executable and required runtime libraries
- NESLogic command-line tool, if enabled
- license and third-party notices
- README, quick-start guide and compatibility matrix
- blank configuration directory; no ROM files by default

## Reproducibility

Pin third-party dependencies, build from a tagged source commit, record compiler/CMake versions, generate checksums, and archive CI logs. Do not describe CI as successful until the workflow has actually run and passed.
