#pragma once

#include "nes/engine/game_engine.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace nex8::studio {

enum class SourceLanguage { assembly6502, cpp23Host, luaHost, neslogic };
struct ProjectFile { std::filesystem::path relativePath; SourceLanguage language{SourceLanguage::assembly6502}; };

struct Project {
    int schemaVersion{1};
    std::string id;
    std::string title;
    std::string templateId{"nex8-homebrew"};
    std::filesystem::path root;
    nex8::engine::CartridgeProject cartridge;
    std::vector<ProjectFile> files;
    std::vector<std::string> tags;
};

struct ValidationIssue {
    bool error{true};
    std::string code;
    std::string message;
    std::filesystem::path path;
};
struct ValidationReport {
    std::vector<ValidationIssue> issues;
    [[nodiscard]] bool ok() const noexcept;
};

[[nodiscard]] ValidationReport validateProject(const Project& project);
[[nodiscard]] std::optional<Project> loadProjectManifest(const std::filesystem::path& path, std::string& error);

} // namespace nex8::studio
