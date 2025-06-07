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
    match(TokenType::KW_INT); // only int for now
    Token nameTok = advance(); // identifier
    match(TokenType::LEFT_PAREN);
    match(TokenType::RIGHT_PAREN);
    auto body = parseBlock();
    auto fn = std::make_unique<FunctionDecl>();
    fn->name = nameTok.lexeme;
    fn->returnType = Type::Int;
    fn->body = std::move(body);
    return fn;
}

std::unique_ptr<BlockStmt> Parser::parseBlock() {
    match(TokenType::LEFT_BRACE);
    auto block = std::make_unique<BlockStmt>();
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        block->statements.push_back(parseStatement());
    }
    match(TokenType::RIGHT_BRACE);
    return block;
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    if (check(TokenType::KW_INT)) return parseVarDecl();
    if (check(TokenType::KW_RETURN)) return parseReturn();
    return parseExprStmt();
}

std::unique_ptr<Stmt> Parser::parseVarDecl() {
    match(TokenType::KW_INT);
    Token nameTok = advance(); // identifier
    std::unique_ptr<Expr> init;
    if (match(TokenType::EQUAL)) {
        init = parseExpression();
    }
    match(TokenType::SEMICOLON);
    auto decl = std::make_unique<VarDecl>();
    decl->name = nameTok.lexeme;
    decl->varType = Type::Int;
    decl->init = std::move(init);
    return decl;
}

std::unique_ptr<Stmt> Parser::parseReturn() {
    match(TokenType::KW_RETURN);
    auto value = parseExpression();
    match(TokenType::SEMICOLON);
    auto stmt = std::make_unique<ReturnStmt>();
    stmt->value = std::move(value);
    return stmt;
}

std::unique_ptr<Stmt> Parser::parseExprStmt() {
    auto expr = parseExpression();
    match(TokenType::SEMICOLON);
    auto stmt = std::make_unique<ExprStmt>();
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
        bin->left = std::move(expr);
        bin->right = std::move(right);
        bin->op = (opTok.type == TokenType::STAR) ? BinaryOp::Mul : BinaryOp::Div;
        expr = std::move(bin);
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().lexeme;
        lit->litType = Type::Int;
        return lit;
    }
    if (match(TokenType::STRING)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().lexeme;
        lit->litType = Type::String;
        return lit;
    }
    if (match(TokenType::IDENTIFIER)) {
        auto id = std::make_unique<Identifier>();
        id->name = previous().lexeme;
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
    printIndent(os, indent); os << "FunctionDecl " << name << " (";
    switch (returnType) {
        case Type::Int: os << "int"; break;
        case Type::Float: os << "float"; break;
        case Type::String: os << "string"; break;
        case Type::Void: os << "void"; break;
    }
    os << ")\n";
    if (body) body->dump(os, indent + 2);
}

void BlockStmt::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "BlockStmt\n";
    for (const auto &s : statements) s->dump(os, indent + 2);
}

void VarDecl::dump(std::ostream &os, int indent) const {
    printIndent(os, indent); os << "VarDecl " << name << " : ";
    switch (varType) {
        case Type::Int: os << "int"; break;
        case Type::Float: os << "float"; break;
        case Type::String: os << "string"; break;
        case Type::Void: os << "void"; break;
    }
    os << "\n";
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
    printIndent(os, indent); os << "Literal " << value << " : ";
    switch (litType) {
        case Type::Int: os << "int"; break;
        case Type::Float: os << "float"; break;
        case Type::String: os << "string"; break;
        case Type::Void: os << "void"; break;
    }
    os << "\n";
}

} // namespace mylang

