#include "neslogic/runtime.hpp"
#include <sstream>
#include <stdexcept>

namespace neslogic {
void Environment::define(std::string name, Value value) {
    values_.insert_or_assign(std::move(name), std::move(value));
}
Value* Environment::get(const std::string& name) {
    if (auto it = values_.find(name); it != values_.end()) return &it->second;
    return parent_ ? parent_->get(name) : nullptr;
}
const Value* Environment::get(const std::string& name) const {
    if (auto it = values_.find(name); it != values_.end()) return &it->second;
    return parent_ ? parent_->get(name) : nullptr;
}
bool Environment::assign(const std::string& name, Value value) {
    if (auto it = values_.find(name); it != values_.end()) {
        it->second = std::move(value); return true;
    }
    return parent_ ? parent_->assign(name, std::move(value)) : false;
}
void Runtime::registerNative(std::string name, Native function) {
    natives_.insert_or_assign(std::move(name), std::move(function));
}
bool Runtime::hasNative(const std::string& name) const { return natives_.contains(name); }
Value Runtime::callNative(const std::string& name, const std::vector<Value>& args) const {
    const auto it = natives_.find(name);
    if (it == natives_.end()) throw std::runtime_error("unknown native function: " + name);
    return it->second(args);
}
std::string toString(const Value& value) {
    if (std::holds_alternative<Nil>(value.data)) return "nil";
    if (const auto* b = std::get_if<bool>(&value.data)) return *b ? "true" : "false";
    if (const auto* i = std::get_if<std::int64_t>(&value.data)) return std::to_string(*i);
    if (const auto* d = std::get_if<double>(&value.data)) { std::ostringstream out; out << *d; return out.str(); }
    if (const auto* s = std::get_if<std::string>(&value.data)) return *s;
    if (std::holds_alternative<std::shared_ptr<Array>>(value.data)) return "<array>";
    if (std::holds_alternative<std::shared_ptr<Table>>(value.data)) return "<table>";
    if (std::holds_alternative<std::shared_ptr<Function>>(value.data)) return "<function>";
    if (std::holds_alternative<NativeFunction>(value.data)) return "<native-function>";
    if (std::holds_alternative<std::shared_ptr<Class>>(value.data)) return "<class>";
    return "<instance>";
}
}
