#include "nes/frontend/library_scan.hpp"
#include "nes/cartridge/ines.hpp"
#include <algorithm>
#include <cctype>
#include <system_error>
namespace nes::frontend {
std::vector<GameEntry> scanRomLibrary(const std::filesystem::path& root) {
    std::vector<GameEntry> entries;
    std::error_code ec;
    if (!std::filesystem::is_directory(root, ec) || ec) return entries;
    std::filesystem::recursive_directory_iterator it(root, std::filesystem::directory_options::skip_permission_denied, ec), end;
    for (; it != end; it.increment(ec)) {
        if (ec) { ec.clear(); continue; }
        if (!it->is_regular_file(ec) || ec) { ec.clear(); continue; }
        auto ext = it->path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (ext != ".nes") continue;
        const auto loaded = loadInes(it->path());
        if (!loaded) continue;
        GameEntry entry;
        entry.path = it->path();
        entry.title = it->path().stem().string();
        entry.mapper = loaded.image.mapperId;
        entry.prgBytes = loaded.image.prgRom.size();
        entry.chrBytes = loaded.image.chrRom.size();
        entry.battery = loaded.image.battery;
        entries.push_back(std::move(entry));
    }
    return entries;
}
}
