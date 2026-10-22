#pragma once

#include "nex8/logic/statements.hpp"

#include <memory>

namespace nex8::engine {

class GameRule final {
public:
    explicit GameRule(std::unique_ptr<logic::Statement> statement)
        : statement_(std::move(statement)) {}

    void evaluate(logic::Variables& variables) const {
        if (statement_) statement_->execute(variables);
    }

private:
    std::unique_ptr<logic::Statement> statement_;
};

inline GameRule makeBounceRule() {
    logic::Condition top(
        logic::var("ball_y"),
        logic::Compare::lessEqual,
        logic::lit(8));
    return GameRule(std::make_unique<logic::If>(
        std::move(top),
        std::make_unique<logic::Action>([](logic::Variables& v) {
            v["ball_vy"] = -v["ball_vy"];
        })));
}

} // namespace nex8::engine
