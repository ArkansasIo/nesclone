# NESClone XMB-style console dashboard

An original horizontal-category / vertical-item dashboard for the 8-bit console emulator. It borrows a general navigation metaphor, not Sony branding or assets.

Categories: Games (local library and recent entries), Cartridges (metadata and save-data tools), Controllers (bindings/input diagnostics), Tools (ROM builder/CHR viewer/disassembler), Settings (display/audio/storage), and System (versions, updates, logs).

The C++ XmbModel is data/navigation only; a graphical renderer and platform input loop are later milestones. A menu item must remain disabled until its service action is implemented. The view must call IEmulatorService, ILibraryService and ISettingsService rather than accessing CPU internals. Keep user ROMs and save data outside source control.