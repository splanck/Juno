#ifndef AST_HPP
#define AST_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace mylang {

// Base AST node
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void dump(std::ostream &os, int indent = 0) const = 0;
};

enum class Type { Int, Float, String, Void };

// Forward declarations
struct Stmt;
struct Expr;

// Program node
struct Program : ASTNode {
    std::vector<std::unique_ptr<ASTNode>> decls; // functions or globals
    void dump(std::ostream &os, int indent = 0) const override;
};

// Function declaration
struct FunctionDecl : ASTNode {
    std::string name;
    Type returnType{Type::Void};
    std::unique_ptr<Stmt> body; // BlockStmt
    void dump(std::ostream &os, int indent = 0) const override;
};

// Statements
struct Stmt : ASTNode {};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    void dump(std::ostream &os, int indent = 0) const override;
};

struct VarDecl : Stmt {
    std::string name;
    Type varType{Type::Int};
    std::unique_ptr<Expr> init;
    void dump(std::ostream &os, int indent = 0) const override;
};

struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> value;
    void dump(std::ostream &os, int indent = 0) const override;
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expr;
    void dump(std::ostream &os, int indent = 0) const override;
};

// Expressions
struct Expr : ASTNode {};

enum class BinaryOp { Add, Sub, Mul, Div };

struct BinaryExpr : Expr {
    BinaryOp op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    void dump(std::ostream &os, int indent = 0) const override;
};

struct Identifier : Expr {
    std::string name;
    void dump(std::ostream &os, int indent = 0) const override;
};

struct Literal : Expr {
    std::string value;
    Type litType{Type::Int};
    void dump(std::ostream &os, int indent = 0) const override;
};

} // namespace mylang

#endif // AST_HPP
