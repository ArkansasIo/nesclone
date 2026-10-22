#include "nex8/engine/game_objects.hpp"
#include "nex8/engine/game_rules.hpp"

#include <cassert>

int main() {
    using namespace nex8::engine;
    Scene scene;
    auto& left = scene.create<Paddle>("Left", 2);
    auto& ball = scene.create<Ball>();
    left.transform().y = 20;
    ball.transform().x = 10;
    ball.transform().y = 20;
    scene.update(1);
    assert(ball.transform().x == 12);

    left.moveUp();
    assert(left.transform().y == 18);

    nex8::logic::Variables variables{{"ball_y", 4}, {"ball_vy", 1}};
    auto rule = makeBounceRule();
    rule.evaluate(variables);
    assert(variables["ball_vy"] == -1);
    return 0;
}
