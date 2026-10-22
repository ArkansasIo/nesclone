#include "nes/cartridge/ines.hpp"
#include "nes/cartridge/nrom.hpp"
#include "nes/controller/controller.hpp"
#include "nes/nes/bus.hpp"
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {
int failures = 0;
void check(bool condition, const char* what) {
    if (!condition) { std::cerr << "FAIL: " << what << '\n'; ++failures; }
}
std::filesystem::path makeRom() {
    auto path = std::filesystem::temp_directory_path() / "nesclone_test.nes";
    std::ofstream out(path, std::ios::binary);
    std::array<unsigned char, 16> header{
        'N','E','S',0x1A, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    out.write(reinterpret_cast<const char*>(header.data()), static_cast<std::streamsize>(header.size()));
    std::vector<unsigned char> prg(16384, 0xEA);
    prg[0x3FFC] = 0x00; prg[0x3FFD] = 0x80;
    out.write(reinterpret_cast<const char*>(prg.data()), static_cast<std::streamsize>(prg.size()));
    std::vector<unsigned char> chr(8192, 0x00);
    out.write(reinterpret_cast<const char*>(chr.data()), static_cast<std::streamsize>(chr.size()));
    return path;
}
}
int main() {
    nes::Bus bus;
    bus.write(0x0001, 0x5A);
    check(bus.read(0x0001) == 0x5A, "RAM read/write");
    check(bus.read(0x0801) == 0x5A, "RAM mirrors every 0x800 bytes");

    nes::Controller pad;
    pad.setButton(nes::Button::A, true);
    pad.setButton(nes::Button::Right, true);
    pad.writeStrobe(1);
    check((pad.read() & 1u) == 1u, "controller strobe exposes A while high");
    pad.writeStrobe(0);
    check((pad.read() & 1u) == 1u, "controller serial first bit A");
    check((pad.read() & 1u) == 0u, "controller serial second bit B");
    for (int i = 0; i < 5; ++i) (void)pad.read();
    check((pad.read() & 1u) == 1u, "controller serial eighth bit Right");

    const auto path = makeRom();
    auto loaded = nes::loadInes(path);
    check(static_cast<bool>(loaded), "load valid iNES ROM");
    if (loaded) {
        check(loaded.image.mapperId == 0, "decode mapper 0");
        check(loaded.image.prgRom.size() == 16384, "decode 16K PRG");
        check(loaded.image.chrRom.size() == 8192, "decode 8K CHR");
        nes::Nrom mapper(loaded.image);
        nes::u8 value = 0;
        check(mapper.cpuRead(0x8000, value) && value == 0xEA, "NROM PRG read");
        check(mapper.cpuRead(0xC000, value) && value == 0xEA, "NROM 16K mirror");
        check(mapper.ppuRead(0x0000, value), "NROM CHR read");
    } else {
        std::cerr << loaded.error << '\n';
    }
    std::filesystem::remove(path);
    if (failures) return 1;
    std::cout << "All NESClone tests passed.\n";
    return 0;
}
