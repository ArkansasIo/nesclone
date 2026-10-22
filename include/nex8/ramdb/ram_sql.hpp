#pragma once

#include "nex8/ramdb/ram_database.hpp"

#include <string>

namespace nex8::ramdb {

struct SqlResult {
    bool ok{false};
    std::string message;
    std::size_t affectedRows{0};
};

// RAMSQL is a deliberately small SQL-like command layer for an 8-bit game runtime.
// It is not a MySQL server and does not require a network or filesystem database.
[[nodiscard]] SqlResult execute(Database& database, const std::string& statement);

} // namespace nex8::ramdb
