#include "nex8/cell8/cell8_db_service.hpp"

#include "nex8/ramdb/ram_binary.hpp"

namespace nex8::cell8 {

DbResponse Cell8DbService::execute(const DbRequest& request) {
    using namespace nex8::ramdb;
    DbResponse response;
    response.status = DbStatus::Busy;

    switch (request.command) {
    case DbCommand::Nop:
        response.status = DbStatus::Ok;
        response.message = "NOP";
        return response;

    case DbCommand::ExecuteSql: {
        if (request.sql.empty()) {
            response.status = DbStatus::Error;
            response.message = "SQL request is empty";
            return response;
        }
        const auto result = ramdb::execute(database_, request.sql);
        response.status = result.ok ? DbStatus::Ok : DbStatus::Error;
        response.affectedRows = result.affectedRows;
        response.message = result.message;
        return response;
    }

    case DbCommand::Insert: {
        auto* table = database_.table(request.table);
        if (!table) {
            response.status = DbStatus::Error;
            response.message = "table not found: " + request.table;
            return response;
        }
        RowId id{};
        std::string error;
        if (!table->insert(request.values, &id, &error)) {
            response.status = DbStatus::Error;
            response.message = error;
            return response;
        }
        response.status = DbStatus::Ok;
        response.affectedRows = 1;
        response.rowId = id;
        return response;
    }

    case DbCommand::Select: {
        const auto* table = database_.table(request.table);
        if (!table) {
            response.status = DbStatus::Error;
            response.message = "table not found: " + request.table;
            return response;
        }
        response.rows = table->selectAll();
        response.affectedRows = static_cast<std::uint16_t>(response.rows.size());
        response.status = DbStatus::Ok;
        return response;
    }

    case DbCommand::Delete: {
        auto* table = database_.table(request.table);
        if (!table) {
            response.status = DbStatus::Error;
            response.message = "table not found: " + request.table;
            return response;
        }
        response.status = table->erase(request.rowId) ? DbStatus::Ok : DbStatus::Error;
        response.affectedRows = response.status == DbStatus::Ok ? 1 : 0;
        if (response.status == DbStatus::Error) response.message = "row not found";
        return response;
    }

    case DbCommand::Update:
        response.status = DbStatus::Error;
        response.message = "typed UPDATE requires named fields; use ExecuteSql for now";
        return response;

    case DbCommand::Save:
    case DbCommand::Load:
    case DbCommand::Clear:
        response.status = DbStatus::Error;
        response.message = "filesystem/state command requires host service binding";
        return response;
    }

    response.status = DbStatus::Error;
    response.message = "unknown DB command";
    return response;
}

void Cell8DbService::reset() {
    database_.clear();
}

} // namespace nex8::cell8
