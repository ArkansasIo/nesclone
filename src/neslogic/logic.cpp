#include "neslogic/logic.hpp"

#include <stdexcept>

namespace neslogic {

Value* Context::get(const std::string& name) {
    const auto it = values_.find(name);
    return it == values_.end() ? nullptr : &it->second;
}

const Value* Context::get(const std::string& name) const {
    const auto it = values_.find(name);
    return it == values_.end() ? nullptr : &it->second;
}

Value Variable::evaluate(const Context& context) const {
    const auto* value = context.get(name_);
    if (value == nullptr) {
        throw std::runtime_error("NESLogic: undefined variable: " + name_);
    }
    return *value;
}

Value BinaryExpression::evaluate(const Context& context) const {
    return op_(left_->evaluate(context), right_->evaluate(context));
}

void Block::execute(Context& context) {
    for (auto& statement : statements_) {
        statement->execute(context);
    }
}

void IfStatement::execute(Context& context) {
    const Value condition = condition_->evaluate(context);
    if (condition.isBool() && condition.asBool()) {
        if (then_) then_->execute(context);
    } else if (else_) {
        else_->execute(context);
    }
}

std::unique_ptr<Expression> LessThan(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) {
    return std::make_unique<BinaryExpression>(std::move(left), std::move(right),
        [](const Value& a, const Value& b) { return Value(a.asNumber() < b.asNumber()); });
}

std::unique_ptr<Expression> GreaterThan(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) {
    return std::make_unique<BinaryExpression>(std::move(left), std::move(right),
        [](const Value& a, const Value& b) { return Value(a.asNumber() > b.asNumber()); });
}

std::unique_ptr<Expression> Equal(std::unique_ptr<Expression> left, std::unique_ptr<Expression> right) {
    return std::make_unique<BinaryExpression>(std::move(left), std::move(right),
        [](const Value& a, const Value& b) { return Value(a.asNumber() == b.asNumber()); });
}

} // namespace neslogic
