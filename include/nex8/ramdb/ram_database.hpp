#pragma once

#include "nex8/ramdb/ram_table.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace nex8::ramdb {

class Database {
public:
    bool createTable(TableSchema schema, std::string* error = nullptr);
    bool dropTable(const std::string& name);
    [[nodiscard]] bool hasTable(const std::string& name) const;
    [[nodiscard]] Table* table(const std::string& name);
    [[nodiscard]] const Table* table(const std::string& name) const;
    [[nodiscard]] std::vector<std::string> tableNames() const;
    void clear();

private:
    std::unordered_map<std::string, Table> tables_;
};

} // namespace nex8::ramdb
