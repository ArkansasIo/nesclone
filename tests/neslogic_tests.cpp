#include "neslogic/logic.hpp"

#include <cassert>
#include <memory>

int main() {
    using namespace neslogic;
    Context context;
    context.set("ball_y", Value(std::int64_t{4}));
    context.set("limit", Value(std::int64_t{8}));
    bool bounced = false;

    auto condition = LessThan(std::make_unique<Variable>("ball_y"), std::make_unique<Variable>("limit"));
    IfStatement rule(std::move(condition), std::make_unique<Action>([&](Context&) { bounced = true; }));
    rule.execute(context);
    assert(bounced);
    return 0;
}
