# Debugger and developer tools

Planned developer views:

- CPU registers/status flags and cycle count
- instruction disassembly and trace logging
- breakpoints, single-step and frame-step
- CPU/PPU memory viewers
- nametable, pattern-table, palette and OAM viewers
- controller state monitor
- cartridge header/mapper summary
- state snapshot and restore
- ROM test harness that captures pass/fail codes and instruction traces

The first deliverable is a command-line runner that validates a ROM and displays mapper, PRG/CHR sizes, mirroring and reset vector. Interactive debugging requires the frontend and remaining hardware subsystems.
