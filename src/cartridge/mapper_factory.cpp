#include "nes/cartridge/mapper_factory.hpp"
#include "nes/cartridge/nrom.hpp"
namespace nes {
MapperResult createMapper(const CartridgeImage& image) {
 if (image.mapperId == 0) return {std::make_shared<Nrom>(image),{}};
 return {nullptr,"unsupported mapper ID: "+std::to_string(image.mapperId)};
}
}