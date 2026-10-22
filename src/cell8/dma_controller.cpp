#include "nex8/bus/dma_controller.hpp"

#include <algorithm>

namespace nex8::bus {

DmaResult DmaController::transfer(std::span<const std::uint8_t> source,
                                  std::span<std::uint8_t> destination,
                                  DmaRequest request) const {
    const auto src = static_cast<std::size_t>(request.source);
    const auto dst = static_cast<std::size_t>(request.destination);
    const auto len = static_cast<std::size_t>(request.length);
    if (len == 0) return {true, 0, {}};
    if (src > source.size() || len > source.size() - src)
        return {false, 0, "DMA source range is outside the source buffer"};
    if (dst > destination.size() || len > destination.size() - dst)
        return {false, 0, "DMA destination range is outside the destination buffer"};
    std::copy_n(source.begin() + static_cast<std::ptrdiff_t>(src), len,
                destination.begin() + static_cast<std::ptrdiff_t>(dst));
    return {true, request.length, {}};
}

} // namespace nex8::bus
