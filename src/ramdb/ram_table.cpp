#include "nex8/ramdb/ram_table.hpp"

namespace nex8::ramdb {

Table::Table(TableSchema schema) : schema_(std::move(schema)), used_(schema_.maxRows, false) {}

bool Table::validateRow(const Row& row, std::string* error) const {
    if (row.size() != schema_.columns.size()) { if (error) *error = "row column count mismatch"; return false; }
    for (std::size_t i = 0; i < row.size(); ++i) {
        if (!valueFits(schema_.columns[i].type, row[i])) {
            if (error) *error = "value does not fit column " + schema_.columns[i].name;
            return false;
        }
        if (schema_.columns[i].type == ValueType::Text && valueToString(row[i]).size() > schema_.columns[i].size) {
            if (error) *error = "text value exceeds column size";
            return false;
        }
    }
    return true;
}

bool Table::insert(const Row& row, RowId* assignedId, std::string* error) {
    if (!validateRow(row, error)) return false;
    for (std::size_t i = 0; i < used_.size(); ++i) {
        if (!used_[i]) {
            used_[i] = true;
            const auto id = static_cast<RowId>(i);
            rows_.push_back(RowRecord{id, row});
            if (assignedId) *assignedId = id;
            return true;
        }
    }
    if (error) *error = "table is full";
    return false;
}

bool Table::insert(const std::unordered_map<std::string, Value>& fields, RowId* assignedId, std::string* error) {
    Row row;
    row.reserve(schema_.columns.size());
    for (const auto& column : schema_.columns) {
        const auto it = fields.find(column.name);
        if (it == fields.end()) { if (error) *error = "missing column " + column.name; return false; }
        row.push_back(it->second);
    }
    return insert(row, assignedId, error);
}

std::optional<std::size_t> Table::storageIndex(RowId id) const {
    if (id >= used_.size() || !used_[id]) return std::nullopt;
    for (std::size_t i = 0; i < rows_.size(); ++i) if (rows_[i].id == id) return i;
    return std::nullopt;
}

bool Table::update(RowId id, const std::unordered_map<std::string, Value>& fields, std::string* error) {
    const auto index = storageIndex(id);
    if (!index) { if (error) *error = "row not found"; return false; }
    Row candidate = rows_[*index].values;
    for (const auto& [name, value] : fields) {
        const auto columnIndex = schema_.columnIndex(name);
        if (columnIndex == schema_.columns.size()) { if (error) *error = "unknown column " + name; return false; }
        candidate[columnIndex] = value;
    }
    if (!validateRow(candidate, error)) return false;
    rows_[*index].values = std::move(candidate);
    return true;
}

bool Table::erase(RowId id) {
    const auto index = storageIndex(id);
    if (!index) return false;
    used_[id] = false;
    rows_.erase(rows_.begin() + static_cast<std::ptrdiff_t>(*index));
    return true;
}

const RowRecord* Table::find(RowId id) const {
    const auto index = storageIndex(id);
    return index ? &rows_[*index] : nullptr;
}

RowRecord* Table::find(RowId id) {
    const auto index = storageIndex(id);
    return index ? &rows_[*index] : nullptr;
}

std::vector<RowRecord> Table::selectAll() const { return rows_; }

std::vector<RowRecord> Table::selectEquals(const std::string& column, const Value& value) const {
    const auto index = schema_.columnIndex(column);
    if (index == schema_.columns.size()) return {};
    std::vector<RowRecord> result;
    for (const auto& row : rows_) if (valueEquals(row.values[index], value)) result.push_back(row);
    return result;
}

void Table::clear() {
    rows_.clear();
    std::fill(used_.begin(), used_.end(), false);
}

} // namespace nex8::ramdb
