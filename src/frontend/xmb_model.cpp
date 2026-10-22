#include "nes/frontend/xmb_model.hpp"
namespace nes::frontend {
XmbModel::XmbModel():categories_{
 {"games","Games",{{"library","Game Library","Browse local homebrew ROMs",true},{"recent","Recently Played","Recent entries",true}}},
 {"cartridges","Cartridges",{{"inspect","Cartridge Inspector","Inspect mapper and ROM metadata",true},{"save-data","Save Data","Manage save data",false}}},
 {"controllers","Controllers",{{"configure","Configure Controller","Bind keyboard or gamepad",false},{"test","Input Test","Inspect controller input",false}}},
 {"tools","Tools",{{"rom-builder","ROM Builder","Build iNES files",false},{"chr-viewer","CHR Viewer","Inspect graphics patterns",false},{"disassembler","6502 Disassembler","Disassemble PRG code",false}}},
 {"settings","Settings",{{"video","Video","Scaling/display options",false},{"audio","Audio","Audio options",false},{"storage","Storage","ROM and save paths",true}}},
 {"system","System",{{"about","About NESClone","Version and project status",true},{"updates","Updates","Release management",false},{"logs","Diagnostics","System logs",false}}}
}{}
const XmbCategory* XmbModel::selectedCategory()const{return categoryIndex_<categories_.size()?&categories_[categoryIndex_]:nullptr;}
const XmbMenuItem* XmbModel::selectedItem()const{const auto*c=selectedCategory();return c&&itemIndex_<c->items.size()?&c->items[itemIndex_]:nullptr;}
void XmbModel::moveCategory(int delta){if(categories_.empty())return;const auto n=static_cast<long long>(categories_.size());auto next=(static_cast<long long>(categoryIndex_)+delta)%n;if(next<0)next+=n;categoryIndex_=static_cast<std::size_t>(next);itemIndex_=0;}
void XmbModel::moveItem(int delta){const auto*c=selectedCategory();if(!c||c->items.empty())return;const auto n=static_cast<long long>(c->items.size());auto next=(static_cast<long long>(itemIndex_)+delta)%n;if(next<0)next+=n;itemIndex_=static_cast<std::size_t>(next);}
}