#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace nex8::engine {

struct Vec2i { std::int32_t x{}; std::int32_t y{}; };
struct Recti { std::int32_t x{}; std::int32_t y{}; std::int32_t width{}; std::int32_t height{}; };

[[nodiscard]] inline bool intersects(const Recti& a, const Recti& b) noexcept {
    return a.x < b.x + b.width && a.x + a.width > b.x &&
           a.y < b.y + b.height && a.y + a.height > b.y;
}

class Component {
public:
    virtual ~Component() = default;
    virtual void onAttach() {}
    virtual void fixedUpdate(std::uint64_t) {}
    virtual void onDetach() {}
};

class Entity {
public:
    explicit Entity(std::string name) : name_(std::move(name)) {}
    virtual ~Entity() = default;
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;
    Entity(Entity&&) noexcept = default;
    Entity& operator=(Entity&&) noexcept = default;

    [[nodiscard]] const std::string& name() const noexcept { return name_; }
    [[nodiscard]] Vec2i position() const noexcept { return position_; }
    void setPosition(Vec2i value) noexcept { position_ = value; }
    [[nodiscard]] Vec2i size() const noexcept { return size_; }
    void setSize(Vec2i value) noexcept { size_ = value; }
    [[nodiscard]] bool active() const noexcept { return active_; }
    void setActive(bool value) noexcept { active_ = value; }
    [[nodiscard]] Recti bounds() const noexcept { return {position_.x, position_.y, size_.x, size_.y}; }

    template<class T, class... Args>
    T& addComponent(Args&&... args) {
        static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
        auto item = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *item;
        components_.push_back(std::move(item));
        ref.onAttach();
        return ref;
    }

    void fixedUpdate(std::uint64_t tick) {
        if (!active_) return;
        for (auto& component : components_) component->fixedUpdate(tick);
    }

private:
    std::string name_;
    Vec2i position_{};
    Vec2i size_{8, 8};
    bool active_{true};
    std::vector<std::unique_ptr<Component>> components_;
};

class Scene {
public:
    explicit Scene(std::string name) : name_(std::move(name)) {}
    [[nodiscard]] const std::string& name() const noexcept { return name_; }

    template<class T = Entity, class... Args>
    T& spawn(Args&&... args) {
        static_assert(std::is_base_of_v<Entity, T>, "T must derive from Entity");
        auto item = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *item;
        entities_.push_back(std::move(item));
        return ref;
    }

    void fixedUpdate(std::uint64_t tick) {
        for (auto& entity : entities_) entity->fixedUpdate(tick);
    }
    [[nodiscard]] const std::vector<std::unique_ptr<Entity>>& entities() const noexcept {
        return entities_;
    }

private:
    std::string name_;
    std::vector<std::unique_ptr<Entity>> entities_;
};

} // namespace nex8::engine
