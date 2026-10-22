#pragma once
#include <cstddef>
#include <string>
#include <vector>
namespace nes::frontend {
struct XmbMenuItem { std::string id,title,description; bool enabled=true; };
struct XmbCategory { std::string id,title; std::vector<XmbMenuItem> items; };
class XmbModel {
public:
 XmbModel();
 [[nodiscard]] const std::vector<XmbCategory>& categories() const { return categories_; }
 [[nodiscard]] std::size_t categoryIndex() const { return categoryIndex_; }
 [[nodiscard]] std::size_t itemIndex() const { return itemIndex_; }
 [[nodiscard]] const XmbCategory* selectedCategory() const;
 [[nodiscard]] const XmbMenuItem* selectedItem() const;
 void moveCategory(int delta);
 void moveItem(int delta);
private:
 std::vector<XmbCategory> categories_;
 std::size_t categoryIndex_=0,itemIndex_=0;
};
}