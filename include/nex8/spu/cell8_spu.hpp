#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace nex8::spu {

enum class Role : std::uint8_t { general, audio, sprite, math, ai, collision, animation, compression };
enum class State : std::uint8_t { idle, ready, running, halted, fault };

struct Task final {
    std::uint16_t id{};
    std::vector<std::uint8_t> code;
    std::vector<std::uint8_t> input;
    bool complete{};
};

class Cell8Spu final {
public:
    static constexpr std::size_t localStoreBytes = 256;

    Cell8Spu(std::uint8_t id = 0, Role role = Role::general);
    [[nodiscard]] std::uint8_t id() const noexcept { return id_; }
    [[nodiscard]] Role role() const noexcept { return role_; }
    [[nodiscard]] State state() const noexcept { return state_; }
    [[nodiscard]] std::uint8_t accumulator() const noexcept { return accumulator_; }
    [[nodiscard]] const std::array<std::uint8_t, localStoreBytes>& localStore() const noexcept { return localStore_; }

    bool loadTask(Task task);
    bool start();
    void tick(std::uint32_t cycles) noexcept;
    void halt() noexcept;
    [[nodiscard]] const Task* currentTask() const noexcept;

private:
    std::uint8_t id_{};
    Role role_{Role::general};
    State state_{State::idle};
    std::uint8_t accumulator_{};
    std::uint16_t pc_{};
    std::array<std::uint8_t, localStoreBytes> localStore_{};
    std::vector<Task> queue_;
    std::size_t currentIndex_{};
};

} // namespace nex8::spu
