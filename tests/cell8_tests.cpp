#include "nex8/bus/dma_controller.hpp"
#include "nex8/cartridge/cartridge_image.hpp"
#include "nex8/cpu/cell8_cpu.hpp"
#include "nex8/gpu/cell8_gpu.hpp"
#include "nex8/hardware/profile.hpp"
#include "nex8/logic/statements.hpp"
#include "nex8/spu/cell8_spu.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

int main() {
    using namespace nex8;

    assert(hardware::Profile::targetClockHz == 8'000'000);
    assert(hardware::decodeAddress(0x8000) == hardware::AddressRegion::cartridgeRom);

    cpu::Cell8Cpu cpu;
    const std::array<std::uint8_t, 6> program{
        static_cast<std::uint8_t>(cpu::Opcode::ldiA), 5,
        static_cast<std::uint8_t>(cpu::Opcode::addAImm), 3,
        static_cast<std::uint8_t>(cpu::Opcode::halt), 0
    };
    assert(cpu.load(0x8000, program));
    const auto run = cpu.run(8);
    assert(run.ok && run.halted);
    assert(cpu.registers().a == 8);

    gpu::Cell8Gpu gpu;
    gpu.writePalette(3, 0x7F);
    assert(gpu.readPalette(3) == 0x7F);
    gpu.writeSprite(0, gpu::Sprite{10, 12, 3, 0});
    assert(gpu.readSprite(0).x == 10);
    gpu.plot(0, 0, 42);
    assert(gpu.framebuffer()[0] == 42);

    spu::Cell8Spu spu0(0, spu::Role::audio);
    spu::Task task;
    task.id = 7;
    task.code = {0x01, 0x02, 0x03};
    assert(spu0.loadTask(task));
    assert(spu0.start());
    spu0.tick(3);
    assert(spu0.state() == spu::State::idle);

    bus::DmaController dma;
    std::array<std::uint8_t, 4> source{1, 2, 3, 4};
    std::array<std::uint8_t, 4> destination{};
    const auto transfer = dma.transfer(source, destination, {0, 0, 4, bus::DmaTarget::videoRam});
    assert(transfer.ok && destination == source);
    const auto rejected = dma.transfer(source, destination, {3, 0, 4, bus::DmaTarget::videoRam});
    assert(!rejected.ok);

    cartridge::Image image;
    image.header.mapper = 0;
    image.prg = {0x01, 0x02, 0x03};
    image.graphics = {0xAA};
    image.audio = {0x55, 0x66};
    image.data = {0x10};
    std::string error;
    const auto packed = cartridge::pack(image, error);
    assert(error.empty() && !packed.empty());
    const auto loaded = cartridge::parse(packed);
    assert(loaded.ok && loaded.image.prg == image.prg &&
           loaded.image.graphics == image.graphics && loaded.image.audio == image.audio);
    auto truncated = packed;
    truncated.pop_back();
    assert(!cartridge::parse(truncated).ok);

    logic::Variables values{{"ball_y", 4}, {"bounce_count", 0}};
    logic::Condition isAbove(std::make_unique<logic::Variable>("ball_y"),
        logic::Compare::less, std::make_unique<logic::Literal>(8));
    logic::If rule(std::move(isAbove), std::make_unique<logic::Action>(
        [](logic::Variables& vars) { ++vars["bounce_count"]; }));
    rule.execute(values);
    assert(values["bounce_count"] == 1);

    return 0;
}
