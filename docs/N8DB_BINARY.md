# N8DB Binary Snapshot Format

N8DB is the native binary snapshot format for the NEX8 RAMDB runtime. It stores table schemas and row data without relying on host compiler struct layout.

## Header

| Offset | Size | Field |
|---:|---:|---|
| 0x00 | 4 | Magic `N8DB` |
| 0x04 | 1 | Format version (`1`) |
| 0x05 | 2 | Table count, little-endian |

## Table record

Each table contains:

1. UTF-8 table name prefixed by a 16-bit byte length.
2. Row size (`u16`).
3. Maximum row count (`u16`).
4. Column count (`u16`).
5. Column records.
6. Row count (`u16`).
7. Row records.

### Column record

- UTF-8 column name with a 16-bit byte length.
- `ValueType` as one byte.
- Column offset (`u16`).
- Column storage size (`u16`).

`ValueType` values are the enum values used by `nex8::ramdb::ValueType`: `UInt8`, `UInt16`, `UInt32`, `Int8`, `Int16`, `Int32`, `Bool`, and `Text`.

### Row record

- Row ID (`u16`).
- One encoded value for each schema column, in schema order.

Text values use a 16-bit byte length followed by UTF-8 bytes.

## Runtime API

```cpp
#include "nex8/ramdb/ram_binary.hpp"

std::string error;
auto image = nex8::ramdb::serialize(database, &error);

nex8::ramdb::Database restored;
nex8::ramdb::deserialize(image.bytes, restored, &error);
```

File helpers are also available:

```cpp
saveBinary(database, "save.n8db", &error);
loadBinary("save.n8db", database, &error);
```

Deserialization is transactional: data is decoded into a temporary database and replaces the destination only after the complete image validates.

## Relationship to the cartridge format

N8DB is a RAM/save-data representation. It is not an iNES `.nes` image and is not the native CELL8 `.n8c` cartridge container.

The intended pipeline is:

```text
RAMSQL tables
    |
    v
N8DB snapshot
    |
    +--> save RAM / development dataset
    |
    +--> cartridge dataset packaging
    |
    v
CELL8 / NEX8 runtime
```
