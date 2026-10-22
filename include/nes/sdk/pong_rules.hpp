#pragma once

#include "nes/sdk/game_object.hpp"

#include <cstdint>

namespace nes::sdk {

struct Score final {
    std::uint16_t left{};
    std::uint16_t right{};
};

[[nodiscard]] inline bool overlapsAabb(Vec2i a, Vec2i aSize, Vec2i b, Vec2i bSize) noexcept {
    return a.x < b.x + bSize.x && a.x + aSize.x > b.x &&
           a.y < b.y + bSize.y && a.y + aSize.y > b.y;
}

inline void clampPaddle(Paddle& paddle, std::int32_t top = 16, std::int32_t bottom = 200) {
    const auto y = paddle.position().y;
    paddle.move(y < top ? top - y : (y > bottom ? bottom - y : 0), top, bottom);
}

} // namespace nes::sdk
