#include "PongGame.hpp"

#include <algorithm>

namespace nesclone::pong {

Paddle::Paddle(std::int16_t x, std::int16_t y) : position_{x, y} {}

void Paddle::moveUp() { moveBy(static_cast<std::int16_t>(-speed_)); }
void Paddle::moveDown() { moveBy(speed_); }

void Paddle::moveBy(std::int16_t deltaY) {
    position_.y = static_cast<std::int16_t>(
        std::clamp<int>(static_cast<int>(position_.y) + static_cast<int>(deltaY), 16, 200));
}

Ball::Ball() { reset(); }

void Ball::update() {
    position_.x = static_cast<std::int16_t>(position_.x + velocity_.x);
    position_.y = static_cast<std::int16_t>(position_.y + velocity_.y);
}

void Ball::reset() {
    position_ = {128, 120};
    velocity_ = {2, 1};
}

void Ball::bounceVertical() {
    velocity_.y = static_cast<std::int16_t>(-velocity_.y);
}

void Ball::bounceHorizontal() {
    velocity_.x = static_cast<std::int16_t>(-velocity_.x);
}

void PongGame::movePlayerOneUp() { player_one_.moveUp(); }
void PongGame::movePlayerOneDown() { player_one_.moveDown(); }
void PongGame::movePlayerTwoUp() { player_two_.moveUp(); }
void PongGame::movePlayerTwoDown() { player_two_.moveDown(); }

void PongGame::update() {
    ball_.update();
    collisions();
}

void PongGame::collisions() {
    const auto p = ball_.position();
    const auto v = ball_.velocity();

    if ((p.y <= 8 && v.y < 0) || (p.y >= 232 && v.y > 0)) {
        ball_.bounceVertical();
    }

    if (p.x < 6) {
        ++right_score_;
        ball_.reset();
    } else if (p.x > 250) {
        ++left_score_;
        ball_.reset();
    }
}

} // namespace nesclone::pong
