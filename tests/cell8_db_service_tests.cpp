#include "nex8/cell8/cell8_db_service.hpp"

#include <cassert>
#include <cstdint>

int main() {
    using namespace nex8::cell8;
    using namespace nex8::ramdb;

    Cell8DbService service;
    auto create = service.execute({DbCommand::ExecuteSql, {},
        "CREATE TABLE PLAYERS COLUMN ID UINT8 COLUMN LEVEL UINT8 COLUMN HP UINT16 ENDTABLE"});
    assert(create.status == DbStatus::Ok);

    auto insert = service.execute({DbCommand::ExecuteSql, {},
        "INSERT INTO PLAYERS VALUES 1 10 500"});
    assert(insert.status == DbStatus::Ok);
    assert(insert.affectedRows == 1);

    auto select = service.execute({DbCommand::Select, "PLAYERS"});
    assert(select.status == DbStatus::Ok);
    assert(select.rows.size() == 1);
    assert(select.rows[0].id == 0);

    auto erase = service.execute({DbCommand::Delete, "PLAYERS", {}, {}, 0});
    assert(erase.status == DbStatus::Ok);
    assert(erase.affectedRows == 1);

    auto missing = service.execute({DbCommand::Select, "MISSING"});
    assert(missing.status == DbStatus::Error);
    return 0;
}
