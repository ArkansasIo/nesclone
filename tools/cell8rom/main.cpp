#include "nex8/cartridge/cartridge_image.hpp"

#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: cell8rom <cartridge.n8c>\n";
        return 2;
    }
    const auto result = nex8::cartridge::load(argv[1]);
    if (!result.ok) {
        std::cerr << "CELL8 cartridge load failed: " << result.error << '\n';
        return 1;
    }
    const auto& h = result.image.header;
    std::cout << "NEX8 CELL8 Cartridge (N8CR v" << static_cast<unsigned>(h.version) << ")\n"
              << "Mapper: " << h.mapper << '\n'
              << "PRG bytes: " << h.prgBytes << '\n'
              << "Graphics bytes: " << h.graphicsBytes << '\n'
              << "Audio bytes: " << h.audioBytes << '\n'
              << "Data bytes: " << h.dataBytes << '\n';
    return 0;
}
