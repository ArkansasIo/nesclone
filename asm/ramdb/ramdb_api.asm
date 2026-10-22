; NEX8 RAMDB / RAMSQL assembly reference
; -----------------------------------------
; These DB_* operations are SDK pseudo-ops/macros for the current host runtime.
; They are NOT native CELL8 opcodes in the provisional ISA.
;
; Dataset RAM window (NES-like cartridge convention):
;   $6000-$60FF  table header
;   $6100-$61FF  column metadata
;   $6200-$7FFF  row storage
;
; Suggested ABI:
;   A  = table/column identifier
;   X/Y = 16-bit address or row id (operation dependent)
;   C  = carry/error flag
;
; ---------------------------------------------------------------------------
; CREATE TABLE PLAYERS
; ---------------------------------------------------------------------------
; DB_CREATE_TABLE PLAYERS
; DB_COLUMN ID,    UINT8
; DB_COLUMN LEVEL, UINT8
; DB_COLUMN HP,    UINT16
; DB_COLUMN XP,    UINT16
; DB_END_TABLE
;
; ---------------------------------------------------------------------------
; INSERT
; ---------------------------------------------------------------------------
; LDI A, #1
; LDI B, #10
; LDI X, #<500       ; pseudo notation: assembler macro layer
; LDI Y, #>500
; DB_INSERT PLAYERS
; ; result: X/Y or runtime ABI contains assigned row id
;
; ---------------------------------------------------------------------------
; SELECT
; ---------------------------------------------------------------------------
; DB_SELECT PLAYERS, ID, #1
; ; result: row cursor / status
;
; ---------------------------------------------------------------------------
; UPDATE
; ---------------------------------------------------------------------------
; DB_UPDATE PLAYERS, HP, #600
;
; ---------------------------------------------------------------------------
; DELETE
; ---------------------------------------------------------------------------
; DB_DELETE PLAYERS, ID, #1
;
; Native implementation path:
;   CELL8 instructions -> cartridge/runtime DB service -> RAM table.
; The runtime must validate types and row bounds before writing RAM.
