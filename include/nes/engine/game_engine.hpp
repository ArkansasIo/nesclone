#pragma once

#include "nes/engine/scene.hpp"

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace nex8::engine {

enum class State { stopped, running, paused };
struct Config { std::uint32_t ticksPerSecond{60}; bool deterministic{true}; };
struct Diagnostic {
    enum class Severity { info, warning, error };
    Severity severity{Severity::info};
    std::string code;
    std::string message;
};

class GameEngine {
public:
    explicit GameEngine(Config config = {});
    [[nodiscard]] const Config& config() const noexcept { return config_; }
    [[nodiscard]] State state() const noexcept { return state_; }
    [[nodiscard]] std::uint64_t tickCount() const noexcept { return tickCount_; }

    Scene& createScene(std::string name);
    bool setActiveScene(const std::string& name);
    [[nodiscard]] Scene* activeScene() noexcept;
    [[nodiscard]] const Scene* activeScene() const noexcept;
    void start() noexcept;
    void pause() noexcept;
    void stop() noexcept;
    void fixedTick();

private:
    Config config_;
    State state_{State::stopped};
    std::uint64_t tickCount_{};
    std::vector<std::unique_ptr<Scene>> scenes_;
    std::size_t activeSceneIndex_{};
};

struct CartridgeProject {
    std::filesystem::path root;
    std::string id;
    std::string title;
    std::uint16_t mapper{};
    std::filesystem::path assemblyEntry;
    std::filesystem::path linkerConfig;
    std::filesystem::path outputRom;
};
struct BuildResult {
    bool success{};
    std::vector<Diagnostic> diagnostics;
    std::filesystem::path output;
};
class ICartridgeBuilder {
public:
    virtual ~ICartridgeBuilder() = default;
    [[nodiscard]] virtual BuildResult build(const CartridgeProject& project) = 0;
};

} // namespace nex8::engine
