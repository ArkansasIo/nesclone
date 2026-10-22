#pragma once
#include "nes/controller/controller.hpp"
#include <map>

namespace nes {
enum class HostKey { Z, X, RightShift, Enter, Up, Down, Left, Right };
class InputMapper {
public:
    void set(HostKey key, bool down, Controller& controller) const;
};
}
