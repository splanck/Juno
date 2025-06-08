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
    char c = source[current++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

bool Lexer::match(char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

void Lexer::skipWhitespace() {
    while (std::isspace(peek())) {
        advance();
    }
}

Token Lexer::makeToken(TokenType type, const std::string &lexeme, int tokLine, int tokColumn) {
    return Token{type, lexeme, tokLine, tokColumn};
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (current < source.size()) {
        skipWhitespace();
        if (current >= source.size()) break;
        int tokLine = line;
        int tokCol = column;
        char c = peek();
        size_t start = current;

        if (std::isalpha(c) || c == '_') {
            while (std::isalnum(peek()) || peek() == '_') advance();
            std::string text = source.substr(start, current - start);
            if (text == "int") {
                tokens.push_back(makeToken(TokenType::KW_INT, text, tokLine, tokCol));
            } else if (text == "float") {
                tokens.push_back(makeToken(TokenType::KW_FLOAT, text, tokLine, tokCol));
            } else if (text == "string") {
                tokens.push_back(makeToken(TokenType::KW_STRING, text, tokLine, tokCol));
            } else if (text == "void") {
                tokens.push_back(makeToken(TokenType::KW_VOID, text, tokLine, tokCol));
            } else if (text == "return") {
                tokens.push_back(makeToken(TokenType::KW_RETURN, text, tokLine, tokCol));
            } else if (text == "if") {
                tokens.push_back(makeToken(TokenType::KW_IF, text, tokLine, tokCol));
            } else if (text == "while") {
                tokens.push_back(makeToken(TokenType::KW_WHILE, text, tokLine, tokCol));
            } else {
                tokens.push_back(makeToken(TokenType::IDENTIFIER, text, tokLine, tokCol));
            }
            continue;
        }

        if (std::isdigit(c)) {
            while (std::isdigit(peek())) advance();
            std::string text = source.substr(start, current - start);
            tokens.push_back(makeToken(TokenType::NUMBER, text, tokLine, tokCol));
            continue;
        }

        switch (advance()) {
            case '(': tokens.push_back(makeToken(TokenType::LEFT_PAREN, "(", tokLine, tokCol)); break;
            case ')': tokens.push_back(makeToken(TokenType::RIGHT_PAREN, ")", tokLine, tokCol)); break;
            case '{': tokens.push_back(makeToken(TokenType::LEFT_BRACE, "{", tokLine, tokCol)); break;
            case '}': tokens.push_back(makeToken(TokenType::RIGHT_BRACE, "}", tokLine, tokCol)); break;
            case ';': tokens.push_back(makeToken(TokenType::SEMICOLON, ";", tokLine, tokCol)); break;
            case '+': tokens.push_back(makeToken(TokenType::PLUS, "+", tokLine, tokCol)); break;
            case '-': tokens.push_back(makeToken(TokenType::MINUS, "-", tokLine, tokCol)); break;
            case '*': tokens.push_back(makeToken(TokenType::STAR, "*", tokLine, tokCol)); break;
            case '/': tokens.push_back(makeToken(TokenType::SLASH, "/", tokLine, tokCol)); break;
            case '=': tokens.push_back(makeToken(TokenType::EQUAL, "=", tokLine, tokCol)); break;
            default: tokens.push_back(makeToken(TokenType::INVALID, std::string(1, c), tokLine, tokCol)); break;
        }
    }

    tokens.push_back(makeToken(TokenType::END_OF_FILE, "", line, column));
    return tokens;
}

} // namespace mylang
