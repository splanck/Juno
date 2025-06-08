#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "ast.hpp"

namespace mylang {

class SemanticAnalyzer {
public:
    bool analyze(const Program &program);

private:
    using Scope = std::unordered_map<std::string, Type>;
    std::vector<Scope> scopes;
    std::vector<std::string> diagnostics;

    void pushScope();
    void popScope();
    bool lookup(const std::string &name, Type &out) const;
    bool typesCompatible(Type a, Type b) const;
    void addDiagnostic(int line, int column, const std::string &msg);

    void analyzeProgram(const Program &program);
    void analyzeFunction(const FunctionDecl &fn);
    void analyzeStmt(const Stmt *stmt, Type expectedReturn);
    Type analyzeExpr(const Expr *expr);
};

} // namespace mylang

#endif // SEMANTIC_ANALYZER_HPP
