#include "nes/frontend/xmb_model.hpp"
#include "nes/frontend/settings_store.hpp"
#include <filesystem>
#include <iostream>
#include <optional>
int main(){
 int failures=0;
 nes::frontend::XmbModel model;
 if(!model.selectedCategory()||model.selectedCategory()->id!="games")++failures;
 model.moveCategory(1);
 if(!model.selectedCategory()||model.selectedCategory()->id!="cartridges")++failures;
 model.moveCategory(-1);model.moveItem(1);
 if(!model.selectedItem()||model.selectedItem()->id!="recent")++failures;
 const auto path=std::filesystem::temp_directory_path()/"nesclone-settings-test.ini";
 nes::frontend::SettingsStore settings(path);
 if(!settings.set("video.scale","3").ok||!settings.save().ok)++failures;
 nes::frontend::SettingsStore loaded(path);
 if(!loaded.load().ok||loaded.get("video.scale")!=std::optional<std::string>("3"))++failures;
 std::error_code ec;std::filesystem::remove(path,ec);
 if(failures){std::cerr<<failures<<" frontend tests failed\n";return 1;}
 std::cout<<"Frontend tests passed\n";return 0;
}