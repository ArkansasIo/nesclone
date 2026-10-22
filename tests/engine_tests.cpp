#include "nes/engine/game_engine.hpp"
#include "nes/studio/project.hpp"

#include <cassert>
#include <stdexcept>

int main() {
    using namespace nex8::engine;
    GameEngine engine;
    auto& scene = engine.createScene("Main");
    auto& player = scene.spawn<Entity>("Player");
    player.setPosition({10, 10});
    player.setSize({16, 16});
    auto& wall = scene.spawn<Entity>("Wall");
    wall.setPosition({20, 20});
    wall.setSize({16, 16});
    assert(intersects(player.bounds(), wall.bounds()));
    assert(engine.setActiveScene("Main"));
    engine.start();
    engine.fixedTick();
    assert(engine.tickCount() == 1);
    engine.pause();
    engine.fixedTick();
    assert(engine.tickCount() == 1);
    engine.start();
    engine.fixedTick();
    assert(engine.tickCount() == 2);
    engine.stop();
    assert(engine.state() == State::stopped);

    nex8::studio::Project project;
    project.id = "pong";
    project.title = "NEX8 Pong";
    project.root = "cartridges/pong";
    project.cartridge.mapper = 0;
    project.cartridge.assemblyEntry = "asm/reset.s";
    project.cartridge.outputRom = "build/pong.nes";
    assert(nex8::studio::validateProject(project).ok());

    bool duplicateCaught = false;
    try { (void)engine.createScene("Main"); } catch (const std::invalid_argument&) { duplicateCaught = true; }
    assert(duplicateCaught);
    return 0;
}
