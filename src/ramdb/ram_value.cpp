#include "nex8/ramdb/ram_value.hpp"

#include <limits>
#include <sstream>

namespace nex8::ramdb {

ValueType valueTypeOf(const Value& value) {
    return std::visit([](const auto& item) -> ValueType {
        using T = std::decay_t<decltype(item)>;
        if constexpr (std::is_same_v<T, std::uint8_t>) return ValueType::UInt8;
        if constexpr (std::is_same_v<T, std::uint16_t>) return ValueType::UInt16;
        if constexpr (std::is_same_v<T, std::uint32_t>) return ValueType::UInt32;
        if constexpr (std::is_same_v<T, std::int8_t>) return ValueType::Int8;
        if constexpr (std::is_same_v<T, std::int16_t>) return ValueType::Int16;
        if constexpr (std::is_same_v<T, std::int32_t>) return ValueType::Int32;
        if constexpr (std::is_same_v<T, bool>) return ValueType::Bool;
        return ValueType::Text;
    }, value);
}

bool valueFits(ValueType type, const Value& value) {
    return std::visit([type](const auto& item) {
        using T = std::decay_t<decltype(item)>;
        switch (type) {
        case ValueType::UInt8:
            return std::is_integral_v<T> && !std::is_same_v<T, bool> &&
                   static_cast<long long>(item) >= 0 && static_cast<unsigned long long>(item) <= 255ULL;
        case ValueType::UInt16:
            return std::is_integral_v<T> && !std::is_same_v<T, bool> &&
                   static_cast<long long>(item) >= 0 && static_cast<unsigned long long>(item) <= 65535ULL;
        case ValueType::UInt32:
            return std::is_integral_v<T> && !std::is_same_v<T, bool> &&
                   static_cast<long long>(item) >= 0;
        case ValueType::Int8:
            return std::is_integral_v<T> && static_cast<long long>(item) >= -128 && static_cast<long long>(item) <= 127;
        case ValueType::Int16:
            return std::is_integral_v<T> && static_cast<long long>(item) >= -32768 && static_cast<long long>(item) <= 32767;
        case ValueType::Int32:
            return std::is_integral_v<T> && !std::is_same_v<T, bool>;
        case ValueType::Bool:
            return std::is_same_v<T, bool>;
        case ValueType::Text:
            return std::is_same_v<T, std::string>;
        }
        return false;
    }, value);
}

std::string valueToString(const Value& value) {
    return std::visit([](const auto& item) {
        std::ostringstream out;
        if constexpr (std::is_same_v<std::decay_t<decltype(item)>, bool>) out << (item ? "true" : "false");
        else out << item;
        return out.str();
    }, value);
}

bool valueEquals(const Value& lhs, const Value& rhs) {
    if (lhs.index() == rhs.index()) return lhs == rhs;
    const bool lhsIntegral = std::holds_alternative<std::uint8_t>(lhs) || std::holds_alternative<std::uint16_t>(lhs) ||
        std::holds_alternative<std::uint32_t>(lhs) || std::holds_alternative<std::int8_t>(lhs) ||
        std::holds_alternative<std::int16_t>(lhs) || std::holds_alternative<std::int32_t>(lhs);
    const bool rhsIntegral = std::holds_alternative<std::uint8_t>(rhs) || std::holds_alternative<std::uint16_t>(rhs) ||
        std::holds_alternative<std::uint32_t>(rhs) || std::holds_alternative<std::int8_t>(rhs) ||
        std::holds_alternative<std::int16_t>(rhs);
    if (!lhsIntegral || !rhsIntegral) return false;
    const auto toI64 = [](const Value& v) -> std::int64_t {
        return std::visit([](const auto& item) -> std::int64_t { return static_cast<std::int64_t>(item); }, v);
    };
    return toI64(lhs) == toI64(rhs);
}

} // namespace nex8::ramdb
