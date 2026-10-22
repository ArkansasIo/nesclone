; CELL8 RAMDB service API
; Service window: $3400-$34FF
; These mnemonics describe the bus-level contract. The current host model
; exposes the same operations through Cell8DbService until IN/OUT are wired.

DB_BASE         EQU $3400
DB_CMD          EQU DB_BASE + $00
DB_STATUS       EQU DB_BASE + $01
DB_TABLE        EQU DB_BASE + $02
DB_ROW_LO       EQU DB_BASE + $12
DB_ROW_HI       EQU DB_BASE + $13
DB_COUNT_LO     EQU DB_BASE + $14
DB_COUNT_HI     EQU DB_BASE + $15
DB_DATA         EQU DB_BASE + $20

DB_NOP          EQU $00
DB_INSERT       EQU $01
DB_SELECT       EQU $02
DB_UPDATE       EQU $03
DB_DELETE       EQU $04
DB_EXEC_SQL     EQU $05
DB_SAVE         EQU $06
DB_LOAD         EQU $07
DB_CLEAR        EQU $08

DB_IDLE         EQU $00
DB_OK           EQU $01
DB_ERROR        EQU $02
DB_BUSY         EQU $03

; Example command sequence:
;
;   LDI A, DB_EXEC_SQL
;   OUT DB_CMD
;   ; write command payload into DB_DATA
;   ; poll DB_STATUS until DB_OK or DB_ERROR
;
; The exact IN/OUT instruction encoding remains provisional in CELL8 ISA v1.
