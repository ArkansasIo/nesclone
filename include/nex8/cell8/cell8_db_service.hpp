#pragma once

#include "nex8/ramdb/ram_database.hpp"
#include "nex8/ramdb/ram_sql.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace nex8::cell8 {

// Host-side model of the CELL8 RAMDB service window.
// The service exposes deterministic command semantics that can later be wired
// to the CELL8 IN/OUT instruction bus without changing the database API.
enum class DbCommand : std::uint8_t {
    Nop = 0x00,
    Insert = 0x01,
    Select = 0x02,
    Update = 0x03,
    Delete = 0x04,
    ExecuteSql = 0x05,
    Save = 0x06,
    Load = 0x07,
    Clear = 0x08
};

enum class DbStatus : std::uint8_t {
    Idle = 0,
    Ok = 1,
    Error = 2,
    Busy = 3
};

struct DbRequest final {
    DbCommand command{DbCommand::Nop};
    std::string table;
    std::string sql;
    std::vector<nex8::ramdb::Value> values;
    std::uint16_t rowId{};
};

struct DbResponse final {
    DbStatus status{DbStatus::Idle};
    std::uint16_t affectedRows{};
    std::uint16_t rowId{};
    std::vector<nex8::ramdb::RowRecord> rows;
    std::string message;
};

class Cell8DbService final {
public:
    static constexpr std::uint16_t BaseAddress = 0x3400;
    static constexpr std::uint16_t EndAddress = 0x34FF;

    [[nodiscard]] DbResponse execute(const DbRequest& request);
    [[nodiscard]] const nex8::ramdb::Database& database() const noexcept { return database_; }
    [[nodiscard]] nex8::ramdb::Database& database() noexcept { return database_; }
    void reset();

private:
    nex8::ramdb::Database database_;
};

} // namespace nex8::cell8
