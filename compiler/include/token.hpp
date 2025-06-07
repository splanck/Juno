#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

namespace mylang {

enum class TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN,
    LEFT_BRACE, RIGHT_BRACE,
    SEMICOLON,
    PLUS, MINUS, STAR, SLASH,
    EQUAL,

    // Literals
    IDENTIFIER, NUMBER, STRING,

    // Keywords
    KW_INT, KW_FLOAT, KW_STRING, KW_VOID,
    KW_RETURN,
    KW_IF, KW_WHILE,

    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;
};

} // namespace mylang

#endif // TOKEN_HPP
