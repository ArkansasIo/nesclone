#include "neslogic/lexer.hpp"
#include <cctype>
#include <unordered_map>

namespace neslogic {
LexResult Lexer::scan(const std::string& source) const {
    LexResult result;
    std::size_t i=0, line=1, col=1;
    auto add=[&](TokenKind kind, std::size_t start, std::size_t startCol) {
        result.tokens.push_back({kind, source.substr(start, i-start), line, startCol});
    };
    const std::unordered_map<std::string,TokenKind> keywords{
        {"local",TokenKind::Local},{"function",TokenKind::Function},{"class",TokenKind::Class},
        {"extends",TokenKind::Extends},{"return",TokenKind::Return},{"if",TokenKind::If},
        {"then",TokenKind::Then},{"elseif",TokenKind::ElseIf},{"else",TokenKind::Else},
        {"end",TokenKind::EndKeyword},{"while",TokenKind::While},{"do",TokenKind::Do},
        {"repeat",TokenKind::Repeat},{"until",TokenKind::Until},{"for",TokenKind::For},
        {"in",TokenKind::In},{"and",TokenKind::And},{"or",TokenKind::Or},{"not",TokenKind::Not},
        {"true",TokenKind::True},{"false",TokenKind::False},{"nil",TokenKind::Nil}
    };
    while (i<source.size()) {
        char c=source[i];
        if (c==' '||c=='\t'||c=='\r') { ++i; ++col; continue; }
        if (c=='\n') { ++i; ++line; col=1; continue; }
        if (c=='-' && i+1<source.size() && source[i+1]=='-') {
            while (i<source.size() && source[i]!='\n') { ++i; ++col; }
            continue;
        }
        const auto start=i, startCol=col;
        if (std::isalpha(static_cast<unsigned char>(c)) || c=='_') {
            while (i<source.size() && (std::isalnum(static_cast<unsigned char>(source[i]))||source[i]=='_')) { ++i; ++col; }
            auto text=source.substr(start,i-start);
            auto it=keywords.find(text);
            result.tokens.push_back({it==keywords.end()?TokenKind::Identifier:it->second,text,line,startCol});
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c))) {
            while (i<source.size() && (std::isalnum(static_cast<unsigned char>(source[i]))||source[i]=='.'||source[i]=='_')) { ++i; ++col; }
            add(TokenKind::Number,start,startCol); continue;
        }
        if (c=='"' || c=='\'') {
            const char quote=c; ++i; ++col;
            std::string value;
            bool closed=false;
            while (i<source.size()) {
                c=source[i++]; ++col;
                if (c==quote) { closed=true; break; }
                if (c=='\n') { ++line; col=1; }
                if (c=='\\' && i<source.size()) {
                    const char escaped=source[i++]; ++col;
                    value.push_back(escaped=='n'?'\n':escaped=='t'?'\t':escaped);
                } else value.push_back(c);
            }
            if (!closed) { result.error="unterminated string at line "+std::to_string(line); return result; }
            result.tokens.push_back({TokenKind::String,value,line,startCol}); continue;
        }
        auto two=[&](char next,TokenKind both,TokenKind single) {
            ++i; ++col;
            if (i<source.size() && source[i]==next) { ++i; ++col; add(both,start,startCol); }
            else add(single,start,startCol);
        };
        switch (c) {
        case '(': ++i;++col;add(TokenKind::LeftParen,start,startCol);break;
        case ')': ++i;++col;add(TokenKind::RightParen,start,startCol);break;
        case '{': ++i;++col;add(TokenKind::LeftBrace,start,startCol);break;
        case '}': ++i;++col;add(TokenKind::RightBrace,start,startCol);break;
        case ',': ++i;++col;add(TokenKind::Comma,start,startCol);break;
        case '.': ++i;++col;add(TokenKind::Dot,start,startCol);break;
        case ':': ++i;++col;add(TokenKind::Colon,start,startCol);break;
        case ';': ++i;++col;add(TokenKind::Semicolon,start,startCol);break;
        case '+': ++i;++col;add(TokenKind::Plus,start,startCol);break;
        case '-': ++i;++col;add(TokenKind::Minus,start,startCol);break;
        case '*': ++i;++col;add(TokenKind::Star,start,startCol);break;
        case '/': ++i;++col;add(TokenKind::Slash,start,startCol);break;
        case '%': ++i;++col;add(TokenKind::Percent,start,startCol);break;
        case '=': two('=',TokenKind::EqualEqual,TokenKind::Equal);break;
        case '!': two('=',TokenKind::BangEqual,TokenKind::Bang);break;
        case '<': two('=',TokenKind::LessEqual,TokenKind::Less);break;
        case '>': two('=',TokenKind::GreaterEqual,TokenKind::Greater);break;
        default: result.error="unexpected character at line "+std::to_string(line)+", column "+std::to_string(col);return result;
        }
    }
    result.tokens.push_back({TokenKind::End,"",line,col});
    return result;
}
}
