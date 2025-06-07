#include "lexer.hpp"
#include <cctype>

namespace mylang {

Lexer::Lexer(const std::string &src) : source(src) {}

char Lexer::peek() const {
    if (current >= source.size()) return '\0';
    return source[current];
}

char Lexer::advance() {
    if (current >= source.size()) return '\0';
    return source[current++];
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        current++;
        return true;
    }
    return false;
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        current++;
    }
}

Token Lexer::makeToken(TokenType type, const std::string &lexeme) {
    return Token{type, lexeme};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (current < source.size()) {
        skipWhitespace();
        if (current >= source.size()) break;
        char c = peek();
        size_t start = current;

        if (std::isalpha(c) || c == '_') {
            while (std::isalnum(peek()) || peek() == '_') advance();
            std::string text = source.substr(start, current - start);
            if (text == "int") {
                tokens.push_back(makeToken(TokenType::KW_INT, text));
            } else if (text == "return") {
                tokens.push_back(makeToken(TokenType::KW_RETURN, text));
            } else {
                tokens.push_back(makeToken(TokenType::IDENTIFIER, text));
            }
            continue;
        }

        if (std::isdigit(c)) {
            while (std::isdigit(peek())) advance();
            std::string text = source.substr(start, current - start);
            tokens.push_back(makeToken(TokenType::NUMBER, text));
            continue;
        }

        switch (advance()) {
            case '(': tokens.push_back(makeToken(TokenType::LEFT_PAREN, "(")); break;
            case ')': tokens.push_back(makeToken(TokenType::RIGHT_PAREN, ")")); break;
            case '{': tokens.push_back(makeToken(TokenType::LEFT_BRACE, "{")); break;
            case '}': tokens.push_back(makeToken(TokenType::RIGHT_BRACE, "}")); break;
            case ';': tokens.push_back(makeToken(TokenType::SEMICOLON, ";")); break;
            case '+': tokens.push_back(makeToken(TokenType::PLUS, "+")); break;
            case '-': tokens.push_back(makeToken(TokenType::MINUS, "-")); break;
            case '*': tokens.push_back(makeToken(TokenType::STAR, "*")); break;
            case '/': tokens.push_back(makeToken(TokenType::SLASH, "/")); break;
            case '=': tokens.push_back(makeToken(TokenType::EQUAL, "=")); break;
            default: tokens.push_back(makeToken(TokenType::INVALID, std::string(1, c))); break;
        }
    }

    tokens.push_back(makeToken(TokenType::END_OF_FILE, ""));
    return tokens;
}

} // namespace mylang
