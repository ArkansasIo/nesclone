#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace nex8::studio {

enum class Panel {
    projectExplorer, sourceEditor, assetBrowser, tileEditor, paletteEditor,
    nametableEditor, logicGraph, cartridgeInspector, buildOutput, debugger,
    emulatorPreview, documentation
};
enum class Command {
    newProject, openProject, saveProject, buildCartridge, runInEmulator,
    pauseEmulator, resetEmulator, stepFrame, inspectCartridge, importAsset,
    openDebugger
};
struct ActionResult {
    bool ok{};
    std::string message;
    std::filesystem::path artifact;
};
struct Layout {
    std::vector<Panel> left{Panel::projectExplorer, Panel::assetBrowser};
    std::vector<Panel> center{Panel::sourceEditor, Panel::logicGraph};
    std::vector<Panel> right{Panel::cartridgeInspector, Panel::documentation};
    std::vector<Panel> bottom{Panel::buildOutput, Panel::debugger};
};

} // namespace nex8::studio
