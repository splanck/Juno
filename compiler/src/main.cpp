#include <fstream>
#include <iostream>
#include <sstream>
#include "lexer.hpp"

using namespace mylang;

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source file>\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Could not open file: " << argv[1] << "\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    for (const auto &tok : tokens) {
        std::cout << static_cast<int>(tok.type) << "\t" << tok.lexeme << "\n";
    }

    return 0;
}
