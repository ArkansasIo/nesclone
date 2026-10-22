#pragma once
#include "nes/frontend/services.hpp"
#include <map>

namespace nes::frontend {
class SettingsStore final : public ISettingsService {
public:
    explicit SettingsStore(std::filesystem::path path);
    [[nodiscard]] std::optional<std::string> get(const std::string& key) const override;
    OperationResult set(const std::string& key, const std::string& value) override;
    OperationResult save() override;
    OperationResult load();
private:
    std::filesystem::path path_;
    std::map<std::string,std::string> values_;
};
}
