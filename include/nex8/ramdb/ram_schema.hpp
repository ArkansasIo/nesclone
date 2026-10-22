#pragma once

#include "nex8/ramdb/ram_value.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace nex8::ramdb {

struct Column {
    std::string name;
    ValueType type{ValueType::UInt8};
    std::uint16_t offset{0};
    std::uint16_t size{1};
};

struct TableSchema {
    std::string name;
    std::uint16_t rowSize{0};
    std::uint16_t maxRows{0};
    std::vector<Column> columns;

    [[nodiscard]] const Column* findColumn(const std::string& columnName) const;
    [[nodiscard]] std::size_t columnIndex(const std::string& columnName) const;
    [[nodiscard]] bool valid(std::string* error = nullptr) const;
};

[[nodiscard]] std::uint16_t valueSize(ValueType type);

} // namespace nex8::ramdb
