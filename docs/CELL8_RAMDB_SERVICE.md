# CELL8 RAMDB Service

The CELL8 database service is the hardware-facing contract around the NEX8 RAMDB runtime.

## Service window

`$3400-$34FF` is reserved for the database service.

| Address | Register | Purpose |
|---:|---|---|
| `$3400` | DB_CMD | command |
| `$3401` | DB_STATUS | status |
| `$3402-$3411` | DB_TABLE | table identifier/name buffer |
| `$3412-$3413` | DB_ROW | row ID |
| `$3414-$3415` | DB_COUNT | affected-row/result count |
| `$3420-$34FF` | DB_DATA | request/response payload |

## Commands

| Opcode | Command |
|---:|---|
| `00` | NOP |
| `01` | INSERT |
| `02` | SELECT |
| `03` | UPDATE |
| `04` | DELETE |
| `05` | EXECUTE SQL |
| `06` | SAVE |
| `07` | LOAD |
| `08` | CLEAR |

The host-side `Cell8DbService` currently implements NOP, INSERT, SELECT, DELETE, and EXECUTE SQL. SAVE/LOAD are intentionally host-service operations rather than filesystem access from the emulated CPU. Typed UPDATE will be added when the bus payload schema for named fields is finalized.

## Status

- `00` Idle
- `01` OK
- `02` Error
- `03` Busy

## Design boundary

The service is not a MySQL server. It is a deterministic embedded relational dataset service for the NEX8 cartridge/runtime. SQL-like syntax is accepted by the host tooling, while the cartridge-facing interface is a compact command/data protocol.

The N8DB serializer provides the persistent binary representation used for development saves and future cartridge dataset packaging.
