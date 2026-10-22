#pragma once
#include "nes/cartridge/ines.hpp"
#include "nes/controller/controller.hpp"
#include "nes/cpu/cpu6502.hpp"
#include "nes/nes/bus.hpp"
#include <memory>
#include <string>

namespace nes {
class NES {
public:
    NES();
    [[nodiscard]] bool loadCartridge(const std::filesystem::path& path, std::string& error);
    void reset();
    void clock();
    [[nodiscard]] Cpu6502& cpu() { return cpu_; }
    [[nodiscard]] Bus& bus() { return bus_; }
    [[nodiscard]] Controller& controller() { return controller_; }
    [[nodiscard]] const CartridgeImage* cartridge() const { return image_ ? &*image_ : nullptr; }
private:
    Bus bus_;
    Controller controller_;
    Cpu6502 cpu_;
    std::shared_ptr<Mapper> mapper_;
    std::optional<CartridgeImage> image_;
};
}
