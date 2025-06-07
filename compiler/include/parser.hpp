#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "token.hpp"

namespace mylang {

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens);

    std::unique_ptr<Program> parseProgram();

private:
    const std::vector<Token> &tokens;
    size_t current{0};

    const Token &peek() const;
    const Token &previous() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    const Token &advance();
    bool isAtEnd() const;

    std::unique_ptr<FunctionDecl> parseFunction();
    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parseVarDecl();
    std::unique_ptr<Stmt> parseReturn();
    std::unique_ptr<Stmt> parseExprStmt();
    std::unique_ptr<BlockStmt> parseBlock();
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseAdd();
    std::unique_ptr<Expr> parseMul();
    std::unique_ptr<Expr> parsePrimary();
};

} // namespace mylang

#endif // PARSER_HPP
