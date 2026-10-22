#pragma once
#include "neslogic/lexer.hpp"
#include <string>
#include <vector>

namespace neslogic {
struct AstNode {
    enum class Kind { Program, Local, Expression, If, While, Repeat, For, Function, Class, Return, Block };
    Kind kind = Kind::Program;
    Token token;
    std::vector<AstNode> children;
    std::string text;
};
struct ParseResult {
    AstNode root;
    std::string error;
    [[nodiscard]] explicit operator bool() const { return error.empty(); }
};
class Parser {
public:
    [[nodiscard]] ParseResult parse(const std::vector<Token>& tokens) const;
};
}
