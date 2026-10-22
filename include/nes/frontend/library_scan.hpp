#pragma once
#include "nes/frontend/services.hpp"
namespace nes::frontend {
[[nodiscard]] std::vector<GameEntry> scanRomLibrary(const std::filesystem::path& root);
}
