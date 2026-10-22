#pragma once
#include "nes/types.hpp"
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
namespace nes::frontend {
struct GameEntry { std::filesystem::path path; std::string title; u16 mapper=0; std::size_t prgBytes=0, chrBytes=0; bool battery=false; };
struct OperationResult { bool ok=false; std::string message; };
class IEmulatorService { public: virtual ~IEmulatorService()=default; virtual OperationResult load(const std::filesystem::path&)=0; virtual OperationResult reset()=0; virtual OperationResult pause(bool)=0; virtual OperationResult stepFrame()=0; virtual bool isLoaded() const=0; virtual bool isPaused() const=0; };
class ILibraryService { public: virtual ~ILibraryService()=default; virtual std::vector<GameEntry> scan(const std::filesystem::path&)=0; virtual OperationResult importRom(const std::filesystem::path&,const std::filesystem::path&)=0; };
class ISettingsService { public: virtual ~ISettingsService()=default; virtual std::optional<std::string> get(const std::string&) const=0; virtual OperationResult set(const std::string&,const std::string&)=0; virtual OperationResult save()=0; };
}