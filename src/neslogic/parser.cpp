#include "neslogic/parser.hpp"
namespace neslogic {
ParseResult Parser::parse(const std::vector<Token>& tokens) const {
    ParseResult result;
    result.root.kind = AstNode::Kind::Program;
    if (tokens.empty() || tokens.back().kind != TokenKind::End) {
        result.error = "token stream is missing end marker";
        return result;
    }
    // Parser scaffolding deliberately rejects nonempty programs until statement/expression
    // nodes are implemented, preventing scripts from appearing to execute when ignored.
    if (tokens.size() > 1) {
        result.error = "NESLogic parser subset not implemented yet; lexer succeeded but execution is disabled";
        return result;
    }
    return result;
}
}
