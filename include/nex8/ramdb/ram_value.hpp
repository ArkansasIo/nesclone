#pragma once

#include <cstdint>
#include <string>
#include <variant>

namespace nex8::ramdb {

enum class ValueType : std::uint8_t {
    UInt8,
    UInt16,
    UInt32,
    Int8,
    Int16,
    Int32,
    Bool,
    Text
};

using Value = std::variant<std::uint8_t, std::uint16_t, std::uint32_t,
                           std::int8_t, std::int16_t, std::int32_t, bool,
                           std::string>;

[[nodiscard]] ValueType valueTypeOf(const Value& value);
[[nodiscard]] bool valueFits(ValueType type, const Value& value);
[[nodiscard]] std::string valueToString(const Value& value);
[[nodiscard]] bool valueEquals(const Value& lhs, const Value& rhs);

} // namespace nex8::ramdb
