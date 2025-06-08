#include "parser.hpp"
#include <iostream>

namespace mylang {

Parser::Parser(const std::vector<Token> &toks) : tokens(toks) {}

const Token &Parser::peek() const { return tokens[current]; }
const Token &Parser::previous() const { return tokens[current - 1]; }

bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }

const Token &Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) { advance(); return true; }
    return false;
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    while (!isAtEnd()) {
        program->decls.push_back(parseFunction());
    }
    return program;
}

std::unique_ptr<FunctionDecl> Parser::parseFunction() {
    Type retType = parseType();
    Token nameTok = advance(); // identifier
    match(TokenType::LEFT_PAREN);
    match(TokenType::RIGHT_PAREN);
    auto body = parseBlock();
    auto fn = std::make_unique<FunctionDecl>();
    fn->line = nameTok.line;
    fn->column = nameTok.column;
    fn->returnType = retType;
    fn->name = nameTok.lexeme;
    fn->body = std::move(body);
    return fn;
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    match(TokenType::LEFT_BRACE);
    auto block = std::make_unique<BlockStmt>();
    block->line = previous().line;
    block->column = previous().column;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        block->statements.push_back(parseStatement());
    }
    match(TokenType::RIGHT_BRACE);
    return block;
}

Type Parser::parseType() {
    if (match(TokenType::KW_INT)) return Type::Int;
    if (match(TokenType::KW_FLOAT)) return Type::Float;
    if (match(TokenType::KW_STRING)) return Type::String;
    if (match(TokenType::KW_VOID)) return Type::Void;
    return Type::Int;
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    if (check(TokenType::KW_INT) || check(TokenType::KW_FLOAT) || check(TokenType::KW_STRING))
        return parseVarDecl();
    if (check(TokenType::KW_RETURN)) return parseReturn();
    return parseExprStmt();
}

std::unique_ptr<Stmt> Parser::parseVarDecl() {
    Type varType = parseType();
    Token nameTok = advance(); // identifier
    std::unique_ptr<Expr> init;
    if (match(TokenType::EQUAL)) {
        init = parseExpression();
    }
    match(TokenType::SEMICOLON);
    auto decl = std::make_unique<VarDecl>();
    decl->line = nameTok.line;
    decl->column = nameTok.column;
    decl->varType = varType;
    decl->name = nameTok.lexeme;
    decl->init = std::move(init);
    return decl;
}

std::unique_ptr<Stmt> Parser::parseReturn() {
    match(TokenType::KW_RETURN);
    Token tok = previous();
    auto value = parseExpression();
    match(TokenType::SEMICOLON);
    auto stmt = std::make_unique<ReturnStmt>();
    stmt->line = tok.line;
    stmt->column = tok.column;
    stmt->value = std::move(value);
    return stmt;
}

std::unique_ptr<Stmt> Parser::parseExprStmt() {
    auto expr = parseExpression();
    match(TokenType::SEMICOLON);
    auto stmt = std::make_unique<ExprStmt>();
    stmt->line = expr ? expr->line : previous().line;
    stmt->column = expr ? expr->column : previous().column;
    stmt->expr = std::move(expr);
    return stmt;
}

std::unique_ptr<Expr> Parser::parseExpression() { return parseAdd(); }

std::unique_ptr<Expr> Parser::parseAdd() {
    auto expr = parseMul();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token opTok = previous();
        auto right = parseMul();
        auto bin = std::make_unique<BinaryExpr>();
        bin->line = opTok.line;
        bin->column = opTok.column;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        bin->op = (opTok.type == TokenType::PLUS) ? BinaryOp::Add : BinaryOp::Sub;
        expr = std::move(bin);
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parseMul() {
    auto expr = parsePrimary();
    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        Token opTok = previous();
        auto right = parsePrimary();
        auto bin = std::make_unique<BinaryExpr>();
        bin->line = opTok.line;
        bin->column = opTok.column;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        bin->op = (opTok.type == TokenType::STAR) ? BinaryOp::Mul : BinaryOp::Div;
        expr = std::move(bin);
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        Token tok = previous();
        auto lit = std::make_unique<Literal>();
        lit->line = tok.line;
        lit->column = tok.column;
        lit->value = tok.lexeme;
        return lit;
    }
    if (match(TokenType::STRING)) {
        Token tok = previous();
        auto lit = std::make_unique<Literal>();
        lit->line = tok.line;
        lit->column = tok.column;
        lit->value = tok.lexeme;
        return lit;
    }
    if (match(TokenType::IDENTIFIER)) {
        Token tok = previous();
        auto id = std::make_unique<Identifier>();
        id->line = tok.line;
        id->column = tok.column;
        id->name = tok.lexeme;
        return id;
    }
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        match(TokenType::RIGHT_PAREN);
        return expr;
    }
    // Fallback literal
    auto invalid = std::make_unique<Literal>();
    invalid->value = "";
    return invalid;
}

// AST dump implementations
static void printIndent(std::ostream &os, int level) {
    for (int i = 0; i < level; ++i) os << ' ';
}

void Program::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "Program\n";
    for (const auto &d : decls) d->dump(os, indent + 2);
}

void FunctionDecl::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "FunctionDecl " << name << " : " << typeToString(returnType) << "\n";
    if (body) body->dump(os, indent + 2);
}

void BlockStmt::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "BlockStmt\n";
    for (const auto &s : statements) s->dump(os, indent + 2);
}

void VarDecl::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "VarDecl " << name << " : " << typeToString(varType) << "\n";
    if (init) init->dump(os, indent + 2);
}

void ReturnStmt::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "ReturnStmt\n";
    if (value) value->dump(os, indent + 2);
}

void ExprStmt::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "ExprStmt\n";
    if (expr) expr->dump(os, indent + 2);
}

void BinaryExpr::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "BinaryExpr";
    switch (op) {
        case BinaryOp::Add: os << " +"; break;
        case BinaryOp::Sub: os << " -"; break;
        case BinaryOp::Mul: os << " *"; break;
        case BinaryOp::Div: os << " /"; break;
    }
    os << "\n";
    if (left) left->dump(os, indent + 2);
    if (right) right->dump(os, indent + 2);
}

void Identifier::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "Identifier " << name << "\n";
}

void Literal::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "Literal " << value << "\n";
}

} // namespace mylang

