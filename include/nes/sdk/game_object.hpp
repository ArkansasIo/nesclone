#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace nes::sdk {

// Host-side game model. This type is not emitted directly into NES PRG-ROM.
struct Vec2i {
    std::int32_t x{};
    std::int32_t y{};
};

class GameObject {
public:
    explicit GameObject(std::string name) : name_(std::move(name)) {}
    virtual ~GameObject() = default;

    GameObject(const GameObject&) = default;
    GameObject& operator=(const GameObject&) = default;
    GameObject(GameObject&&) noexcept = default;
    GameObject& operator=(GameObject&&) noexcept = default;

    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] bool enabled() const noexcept { return enabled_; }
    void setEnabled(bool enabled) noexcept { enabled_ = enabled; }

    virtual void initialize() {}
    virtual void update(std::uint32_t /*fixedTicks*/) {}
    virtual void reset() {}

private:
    std::string name_;
    bool enabled_{true};
};

class Paddle final : public GameObject {
public:
    explicit Paddle(std::string name, std::int32_t x, std::int32_t y)
        : GameObject(std::move(name)), position_{x, y} {}

    void move(std::int32_t deltaY, std::int32_t top, std::int32_t bottom) noexcept {
        position_.y += deltaY;
        if (position_.y < top) position_.y = top;
        if (position_.y > bottom) position_.y = bottom;
    }
    [[nodiscard]] Vec2i position() const noexcept { return position_; }
private:
    Vec2i position_;
};

class Ball final : public GameObject {
public:
    Ball() : GameObject("Ball") { reset(); }

    void reset() override {
        position_ = {128, 120};
        velocity_ = {2, 1};
    }
    void update(std::uint32_t /*fixedTicks*/) override {
        position_.x += velocity_.x;
        position_.y += velocity_.y;
        if (position_.y <= 8 || position_.y >= 232) velocity_.y = -velocity_.y;
    }
    void reverseX() noexcept { velocity_.x = -velocity_.x; }
    void reverseY() noexcept { velocity_.y = -velocity_.y; }
    [[nodiscard]] Vec2i position() const noexcept { return position_; }
    [[nodiscard]] Vec2i velocity() const noexcept { return velocity_; }
    void setPosition(Vec2i position) noexcept { position_ = position; }
    void setVelocity(Vec2i velocity) noexcept { velocity_ = velocity; }
private:
    Vec2i position_{};
    Vec2i velocity_{};
};

} // namespace nes::sdk
