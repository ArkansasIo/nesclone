#pragma once
#include "neslogic/value.hpp"
#include <string>
namespace neslogic {
[[nodiscard]] bool isNumber(const Value&);
[[nodiscard]] double numberValue(const Value&);
[[nodiscard]] bool valuesEqual(const Value&, const Value&);
[[nodiscard]] std::string toString(const Value&);
}