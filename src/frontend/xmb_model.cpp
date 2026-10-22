#include "nes/frontend/xmb_model.hpp"
#include <algorithm>
namespace nes::frontend {
XmbModel::XmbModel() : categories_{
    {"games","Games",{{"library","Game Library","Browse your local homebrew ROMs",true},{"recent","Recently Played","Open recent game entries",true}}},
    {"cartridges","Cartridges",{{"inspect","Cartridge Inspector","Inspect iNES/NES 2.0 metadata",true},{"save-data","Save Data","Manage emulator save files",false}}},
    {"controllers","Controllers",{{"configure","Configure Controller","Bind keyboard or gamepad buttons",false},{"test","Input Test","Inspect the controller serial port",false}}},
    {"tools","Tools",{{"rom-builder","ROM Builder","Build iNES images from PRG and CHR assets",false},{"chr-viewer","CHR Viewer","Inspect pattern-table graphics",false},{"disassembler","6502 Disassembler","Disassemble cartridge machine code",false}}},
    {"settings","Settings",{{"video","Video","Scaling, aspect ratio and display options",false},{"audio","Audio","Audio output and latency",false},{"storage","Storage","ROM and save-data paths",true}}},
    {"system","System",{{"about","About NESClone","Version, license and project status",true},{"updates","Updates","Check published releases",false},{"logs","Diagnostics","View system and emulation logs",false}}}
} {}
const XmbCategory* XmbModel::selectedCategory() const {
    return categoryIndex_ < categories_.size() ? &categories_[categoryIndex_] : nullptr;
}
const XmbMenuItem* XmbModel::selectedItem() const {
    const auto* category=selectedCategory();
    return category && itemIndex_ < category->items.size() ? &category->items[itemIndex_] : nullptr;
}
void XmbModel::clampSelection() {
    if (categories_.empty()) { categoryIndex_=itemIndex_=0; return; }
    categoryIndex_ %= categories_.size();
    const auto count=categories_[categoryIndex_].items.size();
    itemIndex_=count ? itemIndex_%count : 0;
}
void XmbModel::moveCategory(int delta) {
    if (categories_.empty()) return;
    const auto count=static_cast<long long>(categories_.size());
    auto next=(static_cast<long long>(categoryIndex_)+delta)%count;
    if (next<0) next+=count;
    categoryIndex_=static_cast<std::size_t>(next);
    itemIndex_=0;
    clampSelection();
}
void XmbModel::moveItem(int delta) {
    const auto* category=selectedCategory();
    if (!category || category->items.empty()) return;
    const auto count=static_cast<long long>(category->items.size());
    auto next=(static_cast<long long>(itemIndex_)+delta)%count;
    if (next<0) next+=count;
    itemIndex_=static_cast<std::size_t>(next);
}
}
