# NESClone console UI / OS shell

NESClone's “OS” is the user-facing application shell running on Windows, macOS and Linux; it is not a replacement PC kernel or bootable console firmware yet.

## Shell modules
- Dashboard and XMB navigation model
- ROM library scanner and cartridge details
- Emulator service (load/reset/pause/step)
- Input mapper
- Settings store
- Tools launcher
- Updates/about/diagnostics pages
- Logging and crash-report boundary

## UI API contract
All menu items have a stable ID, display title, description, enabled state and service action. Disabled actions should explain they are not ready. UI panels do not access CPU, PPU, mapper or ROM bytes directly; they use service interfaces.

## Accessibility
Provide keyboard-only navigation, visible focus, scalable text, reduced motion, strong contrast and a screen-reader-readable menu tree. Preserve aspect ratio for the 256x240 frame; nearest-neighbor integer scaling is preferred for pixel art.
