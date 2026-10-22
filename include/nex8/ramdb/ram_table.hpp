#pragma once

#include "nex8/ramdb/ram_schema.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace nex8::ramdb {

using Row = std::vector<Value>;
using RowId = std::uint16_t;

struct RowRecord {
    RowId id{0};
    Row values;
};

class Table {
public:
    explicit Table(TableSchema schema);

    [[nodiscard]] const TableSchema& schema() const noexcept { return schema_; }
    [[nodiscard]] std::size_t size() const noexcept { return rows_.size(); }
    [[nodiscard]] bool empty() const noexcept { return rows_.empty(); }

    bool insert(const Row& row, RowId* assignedId = nullptr, std::string* error = nullptr);
    bool insert(const std::unordered_map<std::string, Value>& fields, RowId* assignedId = nullptr, std::string* error = nullptr);
    bool restore(RowId id, const Row& row, std::string* error = nullptr);
    bool update(RowId id, const std::unordered_map<std::string, Value>& fields, std::string* error = nullptr);
    bool erase(RowId id);
    [[nodiscard]] const RowRecord* find(RowId id) const;
    [[nodiscard]] RowRecord* find(RowId id);
    [[nodiscard]] std::vector<RowRecord> selectAll() const;
    [[nodiscard]] std::vector<RowRecord> selectEquals(const std::string& column, const Value& value) const;

    void clear();

private:
    bool validateRow(const Row& row, std::string* error) const;
    std::optional<std::size_t> storageIndex(RowId id) const;

    TableSchema schema_;
    std::vector<RowRecord> rows_;
    std::vector<bool> used_;
};

} // namespace nex8::ramdb
