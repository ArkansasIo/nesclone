#include "nes/nes/nes.hpp"
#include "nes/cartridge/nrom.hpp"
#include <memory>

namespace nes {
NES::NES() : cpu_(bus_) { bus_.connectController(&controller_); bus_.connectApu(&apu_); }
bool NES::loadCartridge(const std::filesystem::path& path, std::string& error) {
    auto loaded = loadInes(path);
    if (!loaded) { error = loaded.error; return false; }
    if (loaded.image.mapperId != 0) {
        error = "Initial milestone supports Mapper 0 (NROM) only; requested mapper " +
            std::to_string(loaded.image.mapperId);
        return false;
    }
    if ((loaded.image.prgRom.size() != 16384 && loaded.image.prgRom.size() != 32768) ||
        (loaded.image.chrRom.size() != 0 && loaded.image.chrRom.size() != 8192) ||
        loaded.image.chrRam.size() > 8192) {
        error = "NROM image has unsupported PRG/CHR size";
        return false;
    }
    image_ = std::move(loaded.image);
    mapper_ = std::make_shared<Nrom>(*image_);
    bus_.connectMapper(mapper_);
    reset();
    return true;
}
void NES::reset() { bus_.resetRam(); if (mapper_) mapper_->reset(); cpu_.reset(); apu_.reset(); }
void NES::clock() { cpu_.clock(); apu_.clock(); }
}
