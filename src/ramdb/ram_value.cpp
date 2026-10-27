#include "nex8/ramdb/ram_value.hpp"

#include <sstream>
#include <type_traits>

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
        if constexpr (std::is_same_v<T, bool>) {
            return type == ValueType::Bool;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return type == ValueType::Text;
        } else if constexpr (std::is_integral_v<T>) {
            const long long asSigned = static_cast<long long>(item);
            const unsigned long long asUnsigned = static_cast<unsigned long long>(item);
            switch (type) {
            case ValueType::UInt8:
                return asSigned >= 0 && asUnsigned <= 255ULL;
            case ValueType::UInt16:
                return asSigned >= 0 && asUnsigned <= 65535ULL;
            case ValueType::UInt32:
                return asSigned >= 0;
            case ValueType::Int8:
                return asSigned >= -128 && asSigned <= 127;
            case ValueType::Int16:
                return asSigned >= -32768 && asSigned <= 32767;
            case ValueType::Int32:
                return asSigned >= -2147483648LL && asSigned <= 2147483647LL;
            default:
                return false;
            }
        } else {
            return false;
        }
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
    const auto integral = [](const Value& value) {
        return std::holds_alternative<std::uint8_t>(value) || std::holds_alternative<std::uint16_t>(value) ||
               std::holds_alternative<std::uint32_t>(value) || std::holds_alternative<std::int8_t>(value) ||
               std::holds_alternative<std::int16_t>(value) || std::holds_alternative<std::int32_t>(value);
    };
    if (!integral(lhs) || !integral(rhs)) return false;
    const auto toI64 = [](const Value& value) -> std::int64_t {
        return std::visit([](const auto& item) -> std::int64_t {
            using T = std::decay_t<decltype(item)>;
            if constexpr (std::is_integral_v<T>) return static_cast<std::int64_t>(item);
            else return 0;
        }, value);
    };
    return toI64(lhs) == toI64(rhs);
}

} // namespace nex8::ramdb
