#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <vector>

#include "ast.hpp"
#include "token.hpp"

namespace mylang {

class Parser {
public:
    explicit Parser(const std::vector<Token> &toks);

    std::unique_ptr<Program> parseProgram();

private:
    const Token &peek() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    const Token &advance();

    std::unique_ptr<FunctionDecl> parseFunction();
    std::unique_ptr<BlockStmt> parseBlock();
    StmtPtr parseStatement();
    StmtPtr parseVarDecl();
    StmtPtr parseReturn();
    ExprPtr parseExpression();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parsePrimary();

    const std::vector<Token> &tokens;
    size_t current{0};
};

} // namespace mylang

#endif // PARSER_HPP
