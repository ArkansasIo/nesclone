#include "nex8/ramdb/ram_database.hpp"
#include "nex8/ramdb/ram_memory.hpp"
#include "nex8/ramdb/ram_sql.hpp"

#include <cassert>
#include <cstdint>
#include <string>

int main() {
    using namespace nex8::ramdb;

    TableSchema schema;
    schema.name = "PLAYERS";
    schema.columns = {
        {"ID", ValueType::UInt8, 0, 1},
        {"LEVEL", ValueType::UInt8, 1, 1},
        {"HP", ValueType::UInt16, 2, 2},
        {"XP", ValueType::UInt16, 4, 2}
    };
    schema.rowSize = 6;
    schema.maxRows = 16;

    Database db;
    std::string error;
    assert(db.createTable(schema, &error));
    auto* players = db.table("PLAYERS");
    assert(players != nullptr);

    RowId id = 0;
    assert(players->insert(Row{std::uint8_t{1}, std::uint8_t{10}, std::uint16_t{500}, std::uint16_t{25000}}, &id, &error));
    assert(id == 0);
    assert(players->find(0) != nullptr);
    assert(players->selectEquals("LEVEL", std::uint8_t{10}).size() == 1);

    assert(players->update(0, {{"HP", std::uint16_t{600}}}, &error));
    assert(std::get<std::uint16_t>(players->find(0)->values[2]) == 600);
    assert(players->erase(0));
    assert(players->find(0) == nullptr);

    Database sqlDb;
    auto result = execute(sqlDb, "CREATE TABLE PLAYERS COLUMN ID UINT8 COLUMN LEVEL UINT8 COLUMN HP UINT16 COLUMN XP UINT16 ENDTABLE");
    assert(result.ok);
    result = execute(sqlDb, "INSERT INTO PLAYERS VALUES 1 10 500 25000");
    assert(result.ok && result.affectedRows == 1);
    result = execute(sqlDb, "SELECT * FROM PLAYERS");
    assert(result.ok && result.affectedRows == 1);

    assert(DatasetRamBegin == 0x6000);
    assert(DatasetRamEnd == 0x7FFF);
    assert(DatasetRamSize == 0x2000);
    return 0;
}
