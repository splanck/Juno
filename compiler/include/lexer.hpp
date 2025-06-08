#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include "token.hpp"

namespace mylang {

class Lexer {
public:
    explicit Lexer(const std::string &source);
    std::vector<Token> tokenize();

private:
    char peek() const;
    char advance();
    bool match(char expected);
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string &lexeme, int line, int column);

    const std::string &source;
    size_t current{0};
    int line{1};
    int column{1};
};

} // namespace mylang

#endif // LEXER_HPP
