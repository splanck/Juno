#ifndef SEMANTIC_HPP
#define SEMANTIC_HPP

#include "ast.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace mylang {

class SemanticAnalyzer {
public:
    bool analyze(Program &program);

private:
    std::vector<std::unordered_map<std::string, Type>> scopes;
    Type currentReturn{Type::Void};
    bool hasError{false};

    void pushScope();
    void popScope();
    void error(const std::string &msg);

    void visitFunction(FunctionDecl &fn);
    void visitBlock(BlockStmt &block);
    void visitStmt(Stmt &stmt);
    Type visitExpr(Expr &expr);
    Type lookup(const std::string &name);
};

} // namespace mylang

#endif // SEMANTIC_HPP
