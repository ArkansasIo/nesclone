#include "neslogic/interpreter.hpp"
#include "neslogic/lexer.hpp"
#include <sstream>
namespace neslogic {
Interpreter::Interpreter() {
    runtime_.registerNative("print", [](const std::vector<Value>& args) -> Value {
        (void)args;
        return Value(Nil{});
    });
}
ExecuteResult Interpreter::execute(const std::string& source) {
    Lexer lexer;
    const auto lexed=lexer.scan(source);
    if (!lexed) return {false,{},lexed.error};
    Parser parser;
    const auto parsed=parser.parse(lexed.tokens);
    if (!parsed) return {false,{},parsed.error};
    return {true,{},{}};
}
}
