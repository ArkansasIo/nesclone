# NEX8 Windows Build Scripts

Run these scripts from the repository root or double-click them from `scripts/`.

## Full build

```bat
scripts\install.bat
```

This configures CMake, compiles Release targets, and runs CTest.

## Individual steps

```bat
scripts\configure.bat
scripts\build.bat
scripts\test.bat
```

## Rebuild

```bat
scripts\rebuild.bat
```

## RAMDB tests

```bat
scripts\run_ramdb_tests.bat
```

## Emulator

```bat
scripts\run_nesclone.bat
```

## Requirements

- Windows 10/11 x64
- CMake 3.25+
- Visual Studio 2022 with Desktop development with C++
- MSVC C++ compiler
- Windows SDK

The scripts deliberately use the Visual Studio generator and build configuration rather than assuming MinGW or Ninja is installed.
