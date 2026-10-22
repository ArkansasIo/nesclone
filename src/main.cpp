#include "nes/nes/nes.hpp"
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: nesclone <rom.nes>\n";
        return 2;
    }
    nes::NES system;
    std::string error;
    if (!system.loadCartridge(argv[1], error)) {
        std::cerr << "ROM load failed: " << error << '\n';
        return 1;
    }
    const auto* image = system.cartridge();
    std::cout << "NESClone initial milestone\n"
              << "Mapper: " << image->mapperId << '\n'
              << "PRG-ROM: " << image->prgRom.size() << " bytes\n"
              << "CHR-ROM: " << image->chrRom.size() << " bytes\n"
              << "CHR-RAM: " << image->chrRam.size() << " bytes\n"
              << "Mirroring: " << (image->fourScreen ? "four-screen" :
                    (image->verticalMirroring ? "vertical" : "horizontal")) << '\n'
              << "CPU reset vector: $" << std::hex << system.cpu().registers().pc << '\n'
              << "Note: graphical PPU/APU and gameplay emulation are not implemented yet.\n";
    return 0;
}
