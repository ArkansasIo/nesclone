#include "nex8/ramdb/ram_schema.hpp"

#include <algorithm>

namespace nex8::ramdb {

std::uint16_t valueSize(ValueType type) {
    switch (type) {
    case ValueType::UInt8:
    case ValueType::Int8:
    case ValueType::Bool: return 1;
    case ValueType::UInt16:
    case ValueType::Int16: return 2;
    case ValueType::UInt32:
    case ValueType::Int32: return 4;
    case ValueType::Text: return 0;
    }
    return 0;
}

const Column* TableSchema::findColumn(const std::string& columnName) const {
    const auto it = std::find_if(columns.begin(), columns.end(), [&](const Column& column) {
        return column.name == columnName;
    });
    return it == columns.end() ? nullptr : &*it;
}

std::size_t TableSchema::columnIndex(const std::string& columnName) const {
    for (std::size_t i = 0; i < columns.size(); ++i) if (columns[i].name == columnName) return i;
    return columns.size();
}

bool TableSchema::valid(std::string* error) const {
    if (name.empty()) { if (error) *error = "table name is empty"; return false; }
    if (maxRows == 0) { if (error) *error = "maxRows must be non-zero"; return false; }
    if (columns.empty()) { if (error) *error = "table has no columns"; return false; }
    std::uint32_t expectedOffset = 0;
    for (const auto& column : columns) {
        if (column.name.empty()) { if (error) *error = "column name is empty"; return false; }
        if (column.type == ValueType::Text) {
            if (column.size == 0) { if (error) *error = "text column requires a fixed size"; return false; }
        } else if (column.size != valueSize(column.type)) {
            if (error) *error = "column size does not match type";
            return false;
        }
        if (column.offset != expectedOffset) { if (error) *error = "columns must be tightly packed"; return false; }
        expectedOffset += column.size;
    }
    if (rowSize != expectedOffset) { if (error) *error = "rowSize does not match columns"; return false; }
    return true;
}

} // namespace nex8::ramdb
