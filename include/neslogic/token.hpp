#pragma once
#include <cstddef>
#include <string>

namespace neslogic {
enum class TokenKind {
    End, Identifier, Number, String,
    LeftParen, RightParen, LeftBrace, RightBrace,
    Comma, Dot, Colon, Semicolon,
    Plus, Minus, Star, Slash, Percent,
    Equal, EqualEqual, Bang, BangEqual, Less, LessEqual, Greater, GreaterEqual,
    And, Or, Not,
    Local, Function, Class, Extends, Return,
    If, Then, ElseIf, Else, EndKeyword,
    While, Do, Repeat, Until, For, In,
    True, False, Nil
};
struct Token {
    TokenKind kind = TokenKind::End;
    std::string lexeme;
    std::size_t line = 1;
    std::size_t column = 1;
};
}
