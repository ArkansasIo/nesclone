#pragma once
#include "neslogic/parser.hpp"
#include "neslogic/runtime.hpp"
#include <string>

namespace neslogic {
struct ExecuteResult {
    bool ok = false;
    std::string output;
    std::string error;
};
class Interpreter {
public:
    Interpreter();
    [[nodiscard]] ExecuteResult execute(const std::string& source);
    [[nodiscard]] Runtime& runtime() { return runtime_; }
private:
    Runtime runtime_;
};
}
