#include "neslogic/value.hpp"
namespace neslogic {
bool isNumber(const Value& v) {
 return std::holds_alternative<std::int64_t>(v.data) || std::holds_alternative<double>(v.data);
}
double numberValue(const Value& v) {
 if (const auto* i=std::get_if<std::int64_t>(&v.data)) return static_cast<double>(*i);
 if (const auto* d=std::get_if<double>(&v.data)) return *d;
 return 0.0;
}
}