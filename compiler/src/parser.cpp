#include "parser.hpp"
#include <iostream>

namespace mylang {

Parser::Parser(const std::vector<Token> &toks) : tokens(toks) {}

const Token &Parser::peek() const {
    if (current >= tokens.size()) return tokens.back();
    return tokens[current];
}

bool Parser::check(TokenType type) const {
    return peek().type == type;
}

const Token &Parser::advance() {
    if (current < tokens.size()) ++current;
    return tokens[current - 1];
}

bool Parser::match(TokenType type) {
    if (check(type)) { advance(); return true; }
    return false;
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    while (!check(TokenType::END_OF_FILE)) {
        program->functions.push_back(parseFunction());
    }
    return program;
}

std::unique_ptr<FunctionDecl> Parser::parseFunction() {
    // assuming return type already matched "int"
    match(TokenType::KW_INT);
    std::string name = advance().lexeme; // identifier
    match(TokenType::LEFT_PAREN); // (
    match(TokenType::RIGHT_PAREN); // )
    auto body = parseBlock();
    auto fn = std::make_unique<FunctionDecl>();
    fn->name = name;
    fn->body = std::move(body);
    return fn;
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    auto block = std::make_unique<BlockStmt>();
    match(TokenType::LEFT_BRACE);
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        block->statements.push_back(parseStatement());
    }
    match(TokenType::RIGHT_BRACE);
    return block;
}

StmtPtr Parser::parseStatement() {
    if (check(TokenType::KW_INT)) return parseVarDecl();
    if (check(TokenType::KW_RETURN)) return parseReturn();
    // expression statement
    auto stmt = std::make_unique<ExprStmt>();
    stmt->expr = parseExpression();
    match(TokenType::SEMICOLON);
    return stmt;
}

StmtPtr Parser::parseVarDecl() {
    match(TokenType::KW_INT);
    std::string name = advance().lexeme; // identifier
    auto decl = std::make_unique<VarDecl>();
    decl->name = name;
    if (match(TokenType::EQUAL)) {
        decl->init = parseExpression();
    }
    match(TokenType::SEMICOLON);
    return decl;
}

StmtPtr Parser::parseReturn() {
    match(TokenType::KW_RETURN);
    auto ret = std::make_unique<ReturnStmt>();
    ret->value = parseExpression();
    match(TokenType::SEMICOLON);
    return ret;
}

ExprPtr Parser::parseExpression() {
    return parseTerm();
}

ExprPtr Parser::parseTerm() {
    auto expr = parseFactor();
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        std::string op = advance().lexeme;
        auto right = parseFactor();
        auto bin = std::make_unique<BinaryExpr>();
        bin->op = op;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        expr = std::move(bin);
    }
    return expr;
}

ExprPtr Parser::parseFactor() {
    auto expr = parsePrimary();
    while (check(TokenType::STAR) || check(TokenType::SLASH)) {
        std::string op = advance().lexeme;
        auto right = parsePrimary();
        auto bin = std::make_unique<BinaryExpr>();
        bin->op = op;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        expr = std::move(bin);
    }
    return expr;
}

ExprPtr Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        auto lit = std::make_unique<Literal>();
        lit->value = tokens[current - 1].lexeme;
        return lit;
    }
    if (match(TokenType::IDENTIFIER)) {
        auto id = std::make_unique<Identifier>();
        id->name = tokens[current - 1].lexeme;
        return id;
    }
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        match(TokenType::RIGHT_PAREN);
        return expr;
    }
    // fallback invalid
    auto lit = std::make_unique<Literal>();
    lit->value = "<invalid>";
    return lit;
}

} // namespace mylang
