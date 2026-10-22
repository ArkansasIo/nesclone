#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace neslogic {
struct Value;
struct Table;
struct Function;
struct Class;
struct Instance;

using Nil = std::monostate;
using Array = std::vector<Value>;
using NativeFunction = Value(*)(const std::vector<Value>&);

struct Table {
    std::unordered_map<std::string, Value> fields;
};

struct Function {
    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::string> bodySource;
};

struct Class {
    std::string name;
    std::shared_ptr<Class> base;
    std::unordered_map<std::string, std::shared_ptr<Function>> methods;
};

struct Instance {
    std::shared_ptr<Class> klass;
    std::unordered_map<std::string, Value> fields;
};

struct Value {
    using Storage = std::variant<Nil, bool, std::int64_t, double, std::string,
        std::shared_ptr<Array>, std::shared_ptr<Table>,
        std::shared_ptr<Function>, NativeFunction,
        std::shared_ptr<Class>, std::shared_ptr<Instance>>;
    Storage data{};

    Value() = default;
    template<class T> explicit Value(T value) : data(std::move(value)) {}
    [[nodiscard]] bool isNil() const { return std::holds_alternative<Nil>(data); }
    [[nodiscard]] bool truthy() const {
        if (isNil()) return false;
        if (const auto* b = std::get_if<bool>(&data)) return *b;
        return true;
    }
};
}
