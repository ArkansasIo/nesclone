#include "PongGame.hpp"

namespace nesclone::pong {
void Ball::update() {
    position_.x = static_cast<std::int16_t>(position_.x + velocity_.x);
    position_.y = static_cast<std::int16_t>(position_.y + velocity_.y);
}

void Ball::reset() {
    position_ = {128, 120};
    velocity_ = {2, 1};
}

void PongGame::update() {
    ball_.update();
    collisions();
}

void PongGame::collisions() {
    const auto y = ball_.position().y;
    if (y < 8 || y > 232) ball_.bounceVertical();

    const auto x = ball_.position().x;
    if (x < 8) {
        ++right_score_;
        ball_.reset();
    } else if (x > 248) {
        ++left_score_;
        ball_.reset();
    }
}
} // namespace nesclone::pong
