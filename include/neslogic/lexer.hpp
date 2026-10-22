#pragma once
#include "neslogic/token.hpp"
#include <string>
#include <vector>

namespace neslogic {
struct LexResult {
    std::vector<Token> tokens;
    std::string error;
    [[nodiscard]] explicit operator bool() const { return error.empty(); }
};
class Lexer {
public:
    [[nodiscard]] LexResult scan(const std::string& source) const;
};
}
