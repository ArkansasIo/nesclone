# Updates and patches

Application releases contain platform-specific binaries; core releases update CPU/PPU/APU/mappers; SDK releases update assembly headers/linker layouts/tools; UI data releases update themes/schema.

## Safe update flow
1. Retrieve release metadata over HTTPS from the official repository.
2. Verify SHA-256 checksums and release signatures when signing is configured.
3. Download to a temporary directory and validate package/version/platform.
4. Preserve user settings, ROM library and save data.
5. Replace application files atomically where supported.
6. Record current and previous versions and support rollback.
7. Never run unverified downloaded scripts or silently delete user data.

Self-update is not implemented. Initially, users should install versioned releases explicitly.

Battery-backed cartridge RAM and emulator save states are separate. Use versioned headers, lengths and checksums; write to a temporary file and replace only after successful flush.