#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nex8::logic {

using Number = std::int64_t;
using Value = Number;
using Variables = std::unordered_map<std::string, Value>;

class Expression {
public:
    virtual ~Expression() = default;
    [[nodiscard]] virtual Value evaluate(const Variables& variables) const = 0;
};
class Literal final : public Expression {
public:
    explicit Literal(Value value) : value_(value) {}
    [[nodiscard]] Value evaluate(const Variables&) const override { return value_; }
private:
    Value value_;
};
class Variable final : public Expression {
public:
    explicit Variable(std::string name) : name_(std::move(name)) {}
    [[nodiscard]] Value evaluate(const Variables& variables) const override {
        const auto it = variables.find(name_);
        return it == variables.end() ? 0 : it->second;
    }
private:
    std::string name_;
};
enum class Compare { equal, notEqual, less, lessEqual, greater, greaterEqual };
class Condition final {
public:
    Condition(std::unique_ptr<Expression> left, Compare compare, std::unique_ptr<Expression> right)
        : left_(std::move(left)), compare_(compare), right_(std::move(right)) {}
    [[nodiscard]] bool evaluate(const Variables& variables) const {
        const auto a = left_->evaluate(variables);
        const auto b = right_->evaluate(variables);
        switch (compare_) {
        case Compare::equal: return a == b;
        case Compare::notEqual: return a != b;
        case Compare::less: return a < b;
        case Compare::lessEqual: return a <= b;
        case Compare::greater: return a > b;
        case Compare::greaterEqual: return a >= b;
        }
        return false;
    }
private:
    std::unique_ptr<Expression> left_;
    Compare compare_;
    std::unique_ptr<Expression> right_;
};

class Statement {
public:
    virtual ~Statement() = default;
    virtual void execute(Variables& variables) const = 0;
};
class Assign final : public Statement {
public:
    Assign(std::string name, std::unique_ptr<Expression> value)
        : name_(std::move(name)), value_(std::move(value)) {}
    void execute(Variables& variables) const override { variables[name_] = value_->evaluate(variables); }
private:
    std::string name_;
    std::unique_ptr<Expression> value_;
};
class Action final : public Statement {
public:
    explicit Action(std::function<void(Variables&)> action) : action_(std::move(action)) {}
    void execute(Variables& variables) const override { action_(variables); }
private:
    std::function<void(Variables&)> action_;
};
class Block final : public Statement {
public:
    void add(std::unique_ptr<Statement> statement) { body_.push_back(std::move(statement)); }
    void execute(Variables& variables) const override {
        for (const auto& statement : body_) statement->execute(variables);
    }
private:
    std::vector<std::unique_ptr<Statement>> body_;
};
class If final : public Statement {
public:
    If(Condition condition, std::unique_ptr<Statement> thenBlock, std::unique_ptr<Statement> elseBlock = {})
        : condition_(std::move(condition)), then_(std::move(thenBlock)), else_(std::move(elseBlock)) {}
    void execute(Variables& variables) const override {
        if (condition_.evaluate(variables)) { if (then_) then_->execute(variables); }
        else if (else_) else_->execute(variables);
    }
private:
    Condition condition_;
    std::unique_ptr<Statement> then_, else_;
};
[[nodiscard]] inline std::unique_ptr<Expression> var(std::string name) {
    return std::make_unique<Variable>(std::move(name));
}
[[nodiscard]] inline std::unique_ptr<Expression> lit(Value value) {
    return std::make_unique<Literal>(value);
}

} // namespace nex8::logic
