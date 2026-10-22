#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace nex8::engine {

struct Transform final {
    std::int16_t x{};
    std::int16_t y{};
};

class GameObject {
public:
    explicit GameObject(std::string name) : name_(std::move(name)) {}
    virtual ~GameObject() = default;
    virtual void initialize() {}
    virtual void update(std::uint32_t ticks) = 0;
    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] bool enabled() const noexcept { return enabled_; }
    void setEnabled(bool value) noexcept { enabled_ = value; }
    [[nodiscard]] Transform& transform() noexcept { return transform_; }
    [[nodiscard]] const Transform& transform() const noexcept { return transform_; }

private:
    std::string name_;
    Transform transform_{};
    bool enabled_{true};
};

class Scene final {
public:
    template<class T, class... Args>
    T& create(Args&&... args) {
        auto object = std::make_unique<T>(std::forward<Args>(args)...);
        auto& reference = *object;
        object->initialize();
        objects_.push_back(std::move(object));
        return reference;
    }

    void update(std::uint32_t ticks) {
        for (const auto& object : objects_) if (object->enabled()) object->update(ticks);
    }

    [[nodiscard]] std::size_t size() const noexcept { return objects_.size(); }

private:
    std::vector<std::unique_ptr<GameObject>> objects_;
};

class Paddle final : public GameObject {
public:
    explicit Paddle(std::string name, std::int16_t speed = 2)
        : GameObject(std::move(name)), speed_(speed) {}
    void update(std::uint32_t) override {}
    void moveUp() noexcept { transform().y = static_cast<std::int16_t>(transform().y - speed_); }
    void moveDown() noexcept { transform().y = static_cast<std::int16_t>(transform().y + speed_); }
private:
    std::int16_t speed_;
};

class Ball final : public GameObject {
public:
    Ball() : GameObject("Ball") {}
    void update(std::uint32_t) override {
        transform().x = static_cast<std::int16_t>(transform().x + velocityX_);
        transform().y = static_cast<std::int16_t>(transform().y + velocityY_);
    }
    void bounceX() noexcept { velocityX_ = static_cast<std::int16_t>(-velocityX_); }
    void bounceY() noexcept { velocityY_ = static_cast<std::int16_t>(-velocityY_); }
private:
    std::int16_t velocityX_{2};
    std::int16_t velocityY_{1};
};

} // namespace nex8::engine
