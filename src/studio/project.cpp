#include "nes/studio/project.hpp"

#include <fstream>
#include <regex>
#include <sstream>

namespace nex8::studio {

bool ValidationReport::ok() const noexcept {
    for (const auto& item : issues) if (item.error) return false;
    return true;
}

ValidationReport validateProject(const Project& project) {
    ValidationReport result;
    const auto error = [&](std::string code, std::string message, std::filesystem::path path = {}) {
        result.issues.push_back({true, std::move(code), std::move(message), std::move(path)});
    };
    static const std::regex safeId("^[a-z0-9][a-z0-9-]*$");
    if (!std::regex_match(project.id, safeId)) error("PROJECT001", "Project ID must contain lowercase letters, digits and hyphens.");
    if (project.title.empty()) error("PROJECT002", "Project title is required.");
    if (project.root.empty()) error("PROJECT003", "Project root path is required.");
    if (project.cartridge.mapper != 0) {
        result.issues.push_back({false, "CARTINFO001",
            "Current cartridge runtime implements mapper 0 only; other mapper values are metadata until implemented.",
            project.root});
    }
    if (project.cartridge.assemblyEntry.empty()) error("CART001", "Assembly entry path is required.");
    if (project.cartridge.outputRom.empty()) error("CART002", "Cartridge output path is required.");

    for (const auto& file : project.files) {
        if (file.relativePath.empty() || file.relativePath.is_absolute()) {
            error("FILE001", "Project file paths must be non-empty and relative.", file.relativePath);
            continue;
        }
        for (const auto& part : file.relativePath) {
            if (part == "..") {
                error("FILE002", "Project source path cannot escape the project root.", file.relativePath);
                break;
            }
        }
    }
    return result;
}

// This project manifest reader extracts scalar string/int fields for the first
// template schema. It is deliberately not a general JSON implementation.
std::optional<Project> loadProjectManifest(const std::filesystem::path& path, std::string& errorText) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        errorText = "Cannot open NEX8 project manifest: " + path.string();
        return std::nullopt;
    }
    std::ostringstream stream;
    stream << file.rdbuf();
    const std::string source = stream.str();
    const auto stringField = [&](const std::string& key) -> std::optional<std::string> {
        const std::regex pattern("\\"" + key + "\\"\\s*:\\s*\\"([^\\"]*)\\"");
        std::smatch match;
        if (!std::regex_search(source, match, pattern)) return std::nullopt;
        return match[1].str();
    };
    const auto intField = [&](const std::string& key) -> std::optional<int> {
        const std::regex pattern("\\"" + key + "\\"\\s*:\\s*(-?[0-9]+)");
        std::smatch match;
        if (!std::regex_search(source, match, pattern)) return std::nullopt;
        try { return std::stoi(match[1].str()); } catch (...) { return std::nullopt; }
    };

    Project project;
    project.root = path.parent_path();
    project.cartridge.root = project.root;
    project.id = stringField("id").value_or("");
    project.title = stringField("title").value_or(stringField("name").value_or(""));
    project.templateId = stringField("template").value_or("nex8-homebrew");
    project.cartridge.id = project.id;
    project.cartridge.title = project.title;
    project.cartridge.mapper = static_cast<std::uint16_t>(intField("mapper").value_or(0));
    project.cartridge.assemblyEntry = stringField("entry").value_or("asm/reset.s");
    project.cartridge.linkerConfig = stringField("linker_config").value_or("sdk/linker/nrom128.cfg");
    project.cartridge.outputRom = stringField("output").value_or("build/game.nes");
    const auto report = validateProject(project);
    if (!report.ok()) {
        errorText = report.issues.front().code + ": " + report.issues.front().message;
        return std::nullopt;
    }
    return project;
}

} // namespace nex8::studio
