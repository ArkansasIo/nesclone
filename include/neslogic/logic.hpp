#pragma once

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "neslogic/value.hpp"

namespace neslogic {

class Context {
public:
    void set(std::string name, Value value) { values_[std::move(name)] = std::move(value); }
    [[nodiscard]] Value* get(const std::string& name);
    [[nodiscard]] const Value* get(const std::string& name) const;

private:
    std::unordered_map<std::string, Value> values_;
};

class Expression {
public:
    virtual ~Expression() = default;
    [[nodiscard]] virtual Value evaluate(const Context& context) const = 0;
};

class Literal final : public Expression {
public:
    explicit Literal(Value value) : value_(std::move(value)) {}
    [[nodiscard]] Value evaluate(const Context&) const override { return value_; }
private:
    Value value_;
};

class Variable final : public Expression {
public:
    explicit Variable(std::string name) : name_(std::move(name)) {}
    [[nodiscard]] Value evaluate(const Context& context) const override;
private:
    std::string name_;
};

class BinaryExpression final : public Expression {
public:
    using Operation = std::function<Value(const Value&, const Value&)>;
    BinaryExpression(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, Operation op)
        : left_(std::move(left)), right_(std::move(right)), op_(std::move(op)) {}
    [[nodiscard]] Value evaluate(const Context& context) const override;
private:
    std::unique_ptr<Expression> left_;
    std::unique_ptr<Expression> right_;
    Operation op_;
};

class Statement {
public:
    virtual ~Statement() = default;
    virtual void execute(Context& context) = 0;
};

class Action final : public Statement {
public:
    explicit Action(std::function<void(Context&)> action) : action_(std::move(action)) {}
    void execute(Context& context) override { action_(context); }
private:
    std::function<void(Context&)> action_;
};

class Block final : public Statement {
public:
    void add(std::unique_ptr<Statement> statement) { statements_.push_back(std::move(statement)); }
    void execute(Context& context) override;
private:
    std::vector<std::unique_ptr<Statement>> statements_;
};

class IfStatement final : public Statement {
public:
    IfStatement(std::unique_ptr<Expression> condition,
                std::unique_ptr<Statement> thenBranch,
                std::unique_ptr<Statement> elseBranch = {})
        : condition_(std::move(condition)), then_(std::move(thenBranch)), else_(std::move(elseBranch)) {}
    void execute(Context& context) override;
private:
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<Statement> then_;
    std::unique_ptr<Statement> else_;
};

[[nodiscard]] std::unique_ptr<Expression> LessThan(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
[[nodiscard]] std::unique_ptr<Expression> GreaterThan(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
[[nodiscard]] std::unique_ptr<Expression> Equal(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);

} // namespace neslogic
