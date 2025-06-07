#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <vector>

namespace mylang {

// Forward declarations
struct Stmt;
struct Expr;
struct BlockStmt;
struct FunctionDecl;

using StmtPtr = std::unique_ptr<Stmt>;
using ExprPtr = std::unique_ptr<Expr>;

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void dump(int indent = 0) const = 0;
};

struct Expr : ASTNode {};
struct Stmt : ASTNode {};

struct Program : ASTNode {
    std::vector<std::unique_ptr<FunctionDecl>> functions;
    void dump(int indent = 0) const override;
};

struct FunctionDecl : ASTNode {
    std::string name;
    std::unique_ptr<BlockStmt> body;
    void dump(int indent = 0) const override;
};

struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
    void dump(int indent = 0) const override;
};

struct VarDecl : Stmt {
    std::string name;
    ExprPtr init;
    void dump(int indent = 0) const override;
};

struct ReturnStmt : Stmt {
    ExprPtr value;
    void dump(int indent = 0) const override;
};

struct ExprStmt : Stmt {
    ExprPtr expr;
    void dump(int indent = 0) const override;
};

struct BinaryExpr : Expr {
    std::string op;
    ExprPtr left;
    ExprPtr right;
    void dump(int indent = 0) const override;
};

struct Identifier : Expr {
    std::string name;
    void dump(int indent = 0) const override;
};

struct Literal : Expr {
    std::string value;
    void dump(int indent = 0) const override;
};

} // namespace mylang

#endif // AST_HPP
