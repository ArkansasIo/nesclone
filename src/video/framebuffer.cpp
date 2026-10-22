#include "nes/video/framebuffer.hpp"
#include <algorithm>
namespace nes {
void Framebuffer::clear(Rgba color) { pixels_.fill(color); }
void Framebuffer::setPixel(std::size_t x, std::size_t y, Rgba color) {
    if (x < Width && y < Height) pixels_[y * Width + x] = color;
}
}
