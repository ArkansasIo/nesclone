#pragma once
#include "nes/cartridge/ines.hpp"
#include "nes/cartridge/mapper.hpp"
#include <memory>
#include <string>
namespace nes {
struct MapperResult { std::shared_ptr<Mapper> mapper; std::string error; explicit operator bool() const { return static_cast<bool>(mapper); } };
[[nodiscard]] MapperResult createMapper(const CartridgeImage& image);
}