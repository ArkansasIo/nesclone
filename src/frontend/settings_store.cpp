#include "nes/frontend/settings_store.hpp"
#include <fstream>
#include <iomanip>

namespace nes::frontend {
SettingsStore::SettingsStore(std::filesystem::path path) : path_(std::move(path)) {}
std::optional<std::string> SettingsStore::get(const std::string& key) const {
    const auto it=values_.find(key);
    if(it==values_.end()) return std::nullopt;
    return it->second;
}
OperationResult SettingsStore::set(const std::string& key, const std::string& value) {
    if(key.empty()) return {false,"Setting key cannot be empty"};
    values_.insert_or_assign(key,value);
    return {true,"Setting updated"};
}
OperationResult SettingsStore::save() {
    std::error_code ec;
    if(!path_.parent_path().empty()) std::filesystem::create_directories(path_.parent_path(),ec);
    if(ec) return {false,"Could not create settings directory: "+ec.message()};
    auto temp=path_; temp += ".tmp";
    {
        std::ofstream out(temp,std::ios::trunc);
        if(!out) return {false,"Could not write settings file"};
        out << "# NESClone settings v1\n";
        for(const auto& [k,v]:values_) out << std::quoted(k) << ' ' << std::quoted(v) << '\n';
        if(!out) return {false,"Failed while writing settings"};
    }
    std::filesystem::rename(temp,path_,ec);
    if(ec) {
        std::filesystem::remove(path_,ec); ec.clear();
        std::filesystem::rename(temp,path_,ec);
    }
    if(ec) return {false,"Could not replace settings file: "+ec.message()};
    return {true,"Settings saved"};
}
OperationResult SettingsStore::load() {
    std::ifstream in(path_);
    if(!in) {
        if(!std::filesystem::exists(path_)) return {true,"No settings file yet"};
        return {false,"Could not open settings file"};
    }
    std::map<std::string,std::string> loaded;
    std::string line;
    while(std::getline(in,line)) {
        if(line.empty() || line[0]=='#') continue;
        std::istringstream row(line);
        std::string key,value;
        if(row >> std::quoted(key) >> std::quoted(value)) loaded.insert_or_assign(key,value);
    }
    values_=std::move(loaded);
    return {true,"Settings loaded"};
}
}
