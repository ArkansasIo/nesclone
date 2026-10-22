#include "nex8/ramdb/ram_database.hpp"

#include <algorithm>

namespace nex8::ramdb {

bool Database::createTable(TableSchema schema, std::string* error) {
    if (!schema.valid(error)) return false;
    if (tables_.contains(schema.name)) { if (error) *error = "table already exists"; return false; }
    const auto name = schema.name;
    tables_.emplace(name, Table(std::move(schema)));
    return true;
}

bool Database::dropTable(const std::string& name) { return tables_.erase(name) != 0; }

bool Database::hasTable(const std::string& name) const { return tables_.contains(name); }

Table* Database::table(const std::string& name) {
    const auto it = tables_.find(name);
    return it == tables_.end() ? nullptr : &it->second;
}

const Table* Database::table(const std::string& name) const {
    const auto it = tables_.find(name);
    return it == tables_.end() ? nullptr : &it->second;
}

std::vector<std::string> Database::tableNames() const {
    std::vector<std::string> result;
    result.reserve(tables_.size());
    for (const auto& [name, table] : tables_) { (void)table; result.push_back(name); }
    std::sort(result.begin(), result.end());
    return result;
}

void Database::clear() { tables_.clear(); }

} // namespace nex8::ramdb
