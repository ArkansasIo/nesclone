#pragma once

#include "nex8/ramdb/ram_database.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace nex8::ramdb {

// Deterministic binary snapshot format for cartridge/RAM save data.
// The format is intentionally self-contained and does not depend on host ABI layout.
struct BinaryImage {
    std::vector<std::uint8_t> bytes;
};

[[nodiscard]] BinaryImage serialize(const Database& database, std::string* error = nullptr);
[[nodiscard]] bool deserialize(const std::vector<std::uint8_t>& bytes,
                               Database& database,
                               std::string* error = nullptr);

[[nodiscard]] bool saveBinary(const Database& database,
                              const std::string& path,
                              std::string* error = nullptr);
[[nodiscard]] bool loadBinary(const std::string& path,
                              Database& database,
                              std::string* error = nullptr);

} // namespace nex8::ramdb
