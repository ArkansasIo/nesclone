#include "neslogic/interpreter.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: neslogic <script.nsl>\n";
        return 2;
    }
    std::ifstream file(argv[1]);
    if (!file) { std::cerr << "Cannot open script: " << argv[1] << '\n'; return 1; }
    const std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    neslogic::Interpreter interpreter;
    const auto result = interpreter.execute(source);
    if (!result.ok) { std::cerr << "NESLogic error: " << result.error << '\n'; return 1; }
    std::cout << result.output;
    return 0;
}
