#pragma once

#include <cstdint>

namespace nesclone::pong {
struct Vec2 { std::int16_t x = 0; std::int16_t y = 0; };

class Paddle {
public:
    explicit Paddle(std::int16_t x = 16) : position_{x, 104} {}
    void moveUp() { position_.y -= speed_; }
    void moveDown() { position_.y += speed_; }
    [[nodiscard]] Vec2 position() const { return position_; }
private:
    Vec2 position_;
    std::int16_t speed_ = 2;
};

class Ball {
public:
    Ball() = default;
    void update();
    void reset();
    void bounceVertical() { velocity_.y = static_cast<std::int16_t>(-velocity_.y); }
    void bounceHorizontal() { velocity_.x = static_cast<std::int16_t>(-velocity_.x); }
    [[nodiscard]] Vec2 position() const { return position_; }
    [[nodiscard]] Vec2 velocity() const { return velocity_; }
private:
    Vec2 position_{128, 120};
    Vec2 velocity_{2, 1};
};

class PongGame {
public:
    void update();
    Paddle& playerOne() { return player_one_; }
    Paddle& playerTwo() { return player_two_; }
    Ball& ball() { return ball_; }
    [[nodiscard]] int leftScore() const { return left_score_; }
    [[nodiscard]] int rightScore() const { return right_score_; }
private:
    void collisions();
    Paddle player_one_{16};
    Paddle player_two_{232};
    Ball ball_;
    int left_score_ = 0;
    int right_score_ = 0;
};
} // namespace nesclone::pong
