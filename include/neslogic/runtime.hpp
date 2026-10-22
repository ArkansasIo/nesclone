#pragma once
#include "neslogic/value.hpp"
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace neslogic {
struct RuntimeError {
    std::string message;
    std::size_t line = 0;
};
class Environment {
public:
    explicit Environment(Environment* parent = nullptr) : parent_(parent) {}
    void define(std::string name, Value value);
    [[nodiscard]] Value* get(const std::string& name);
    [[nodiscard]] const Value* get(const std::string& name) const;
    [[nodiscard]] bool assign(const std::string& name, Value value);
private:
    Environment* parent_;
    std::unordered_map<std::string, Value> values_;
};
class Runtime {
public:
    using Native = std::function<Value(const std::vector<Value>&)>;
    void registerNative(std::string name, Native function);
    [[nodiscard]] bool hasNative(const std::string& name) const;
    [[nodiscard]] Value callNative(const std::string& name, const std::vector<Value>& args) const;
private:
    std::unordered_map<std::string, Native> natives_;
};
[[nodiscard]] std::string toString(const Value& value);
}
