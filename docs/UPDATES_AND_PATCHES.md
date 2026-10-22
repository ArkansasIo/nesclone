# Updates, patches and release packages

## Update types

- Application release: signed/versioned binaries for Windows, macOS and Linux.
- Data update: theme presets and metadata schemas.
- Emulator-core update: CPU, PPU, APU, mapper and compatibility changes.
- SDK update: linker config, assembly includes, examples and tool versions.

## Safe update flow

1. Fetch release metadata over HTTPS from the official repository release page.
2. Verify the asset checksum; for signed releases, verify the signature against a pinned project key.
3. Download to a temporary directory.
4. Validate package format and version before extraction.
5. Preserve user settings, ROM files and save data.
6. Apply update atomically where the operating system allows.
7. Record the previous version and provide rollback.
8. Never execute a patch script downloaded without verification.

The initial release should not implement self-modifying executable patching. GitHub Actions builds and publishes artifacts; the user explicitly installs releases until integrity verification and rollback have tests.

## Save data

- Keep emulator states separate from battery-backed cartridge RAM saves.
- Use versioned formats with magic, version, lengths and checksums.
- Write to temporary files and rename only after successful flush.
- Never delete user data during update or uninstall without explicit confirmation.
