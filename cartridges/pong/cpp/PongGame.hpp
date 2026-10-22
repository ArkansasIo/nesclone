#pragma once

#include <cstdint>

namespace nesclone::pong {

struct Vec2 {
    std::int16_t x{};
    std::int16_t y{};
};

class GameObject {
public:
    virtual ~GameObject() = default;
    virtual void update() = 0;
    [[nodiscard]] virtual Vec2 position() const = 0;
};

class Paddle final : public GameObject {
public:
    explicit Paddle(std::int16_t x = 16, std::int16_t y = 104);
    void moveUp();
    void moveDown();
    void moveBy(std::int16_t deltaY);
    void update() override {}
    [[nodiscard]] Vec2 position() const override { return position_; }
private:
    Vec2 position_;
    std::int16_t speed_{2};
};

class Ball final : public GameObject {
public:
    Ball();
    void update() override;
    void reset();
    void bounceVertical();
    void bounceHorizontal();
    [[nodiscard]] Vec2 position() const override { return position_; }
    [[nodiscard]] Vec2 velocity() const noexcept { return velocity_; }
private:
    Vec2 position_{128, 120};
    Vec2 velocity_{2, 1};
};

class PongGame final {
public:
    void update();
    void movePlayerOneUp();
    void movePlayerOneDown();
    void movePlayerTwoUp();
    void movePlayerTwoDown();
    [[nodiscard]] const Paddle& playerOne() const noexcept { return player_one_; }
    [[nodiscard]] const Paddle& playerTwo() const noexcept { return player_two_; }
    [[nodiscard]] const Ball& ball() const noexcept { return ball_; }
    [[nodiscard]] int leftScore() const noexcept { return left_score_; }
    [[nodiscard]] int rightScore() const noexcept { return right_score_; }
private:
    void collisions();
    Paddle player_one_{16, 104};
    Paddle player_two_{232, 104};
    Ball ball_{};
    int left_score_{};
    int right_score_{};
};

} // namespace nesclone::pong
