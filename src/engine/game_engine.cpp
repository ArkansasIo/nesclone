#include "nes/engine/game_engine.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace nex8::engine {

GameEngine::GameEngine(Config config) : config_(config) {
    if (config_.ticksPerSecond == 0) {
        throw std::invalid_argument("NEX8 engine ticksPerSecond must be greater than zero");
    }
}

Scene& GameEngine::createScene(std::string name) {
    if (name.empty()) throw std::invalid_argument("NEX8 scene name must not be empty");
    const auto existing = std::find_if(scenes_.begin(), scenes_.end(),
        [&](const auto& scene) { return scene->name() == name; });
    if (existing != scenes_.end()) throw std::invalid_argument("NEX8 scene names must be unique");
    scenes_.push_back(std::make_unique<Scene>(std::move(name)));
    return *scenes_.back();
}

bool GameEngine::setActiveScene(const std::string& name) {
    for (std::size_t i = 0; i < scenes_.size(); ++i) {
        if (scenes_[i]->name() == name) {
            activeSceneIndex_ = i;
            return true;
        }
    }
    return false;
}

Scene* GameEngine::activeScene() noexcept {
    return activeSceneIndex_ < scenes_.size() ? scenes_[activeSceneIndex_].get() : nullptr;
}
const Scene* GameEngine::activeScene() const noexcept {
    return activeSceneIndex_ < scenes_.size() ? scenes_[activeSceneIndex_].get() : nullptr;
}

void GameEngine::start() noexcept {
    if (!scenes_.empty()) state_ = State::running;
}
void GameEngine::pause() noexcept {
    if (state_ == State::running) state_ = State::paused;
}
void GameEngine::stop() noexcept { state_ = State::stopped; }

void GameEngine::fixedTick() {
    if (state_ != State::running) return;
    if (auto* scene = activeScene()) scene->fixedUpdate(tickCount_);
    ++tickCount_;
}

} // namespace nex8::engine
