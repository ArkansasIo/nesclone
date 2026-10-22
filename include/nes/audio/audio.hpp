#pragma once
#include "nes/types.hpp"
#include <vector>

namespace nes {
struct AudioBuffer {
    u32 sampleRate = 44100;
    std::vector<float> samples;
    void clear() { samples.clear(); }
};
}
