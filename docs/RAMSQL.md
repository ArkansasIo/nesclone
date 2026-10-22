# NEX8 RAMSQL

NEX8 RAMSQL is a small relational dataset layer designed for an NES-like 8-bit memory model. It stores structured game data in RAM and exposes SQL-like operations to the host runtime and SDK.

It is **not MySQL** and does not run a MySQL server. The MySQL terminology is used for the relational-table programming model: tables, columns, rows, insert/select/update/delete, and typed values.

## Memory model

The native cartridge convention reserves `$6000-$7FFF` as a 8 KiB dataset-RAM window:

| Range | Purpose |
|---|---|
| `$6000-$60FF` | Table header |
| `$6100-$61FF` | Column metadata |
| `$6200-$7FFF` | Row storage |

This convention is separate from the legacy iNES/6502 implementation. A future cartridge mapper may expand the dataset region beyond this initial 8 KiB window.

## C++ runtime

The host-side implementation is under `include/nex8/ramdb` and `src/ramdb`:

- `ram_value.*` — typed 8/16/32-bit and text values.
- `ram_schema.*` — fixed-layout table schemas and column metadata.
- `ram_table.*` — row insertion, lookup, update, deletion, and equality selection.
- `ram_database.*` — collection of named tables.
- `ram_sql.*` — small SQL-like command layer.
- `ram_memory.hpp` — dataset address constants and binary header definition.

## Example schema

```text
CREATE TABLE PLAYERS
COLUMN ID UINT8
COLUMN LEVEL UINT8
COLUMN HP UINT16
COLUMN XP UINT16
ENDTABLE
```

The current parser also accepts:

```text
INSERT INTO PLAYERS VALUES 1 10 500 25000
SELECT * FROM PLAYERS
```

The parser is intentionally small. It is suitable for game data and tooling, not arbitrary SQL compatibility.

## Assembly boundary

`asm/ramdb/ramdb_api.asm` documents `DB_CREATE_TABLE`, `DB_INSERT`, `DB_SELECT`, `DB_UPDATE`, and `DB_DELETE` as SDK pseudo-operations.

Those names are **not** being added to the provisional native CELL8 ISA. They represent a cartridge/runtime service ABI that can later be lowered by the assembler/linker into real CELL8 instructions and service calls.

## Lua/NESLogic direction

A future scripting bridge can expose the same database through a Lua-like API:

```lua
players = ramdb.open("PLAYERS")
players:insert({ id = 1, level = 10, hp = 500, xp = 25000 })
player = players:select(1)
player.hp = player.hp + 100
players:update(player)
```

The script should compile to a compact bytecode or host runtime calls. The 8-bit CPU does not directly execute Lua source.

## Design constraints

1. Fixed-size rows are preferred for deterministic memory use.
2. Integer types map directly to compact 8/16/32-bit storage.
3. Text columns have an explicit fixed byte capacity.
4. Table capacity is bounded by `maxRows`.
5. Bounds and type validation happen before writes.
6. The implementation must remain usable without a filesystem or network database.
