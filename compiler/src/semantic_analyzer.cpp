#include "semantic_analyzer.hpp"

#include <iostream>
#include <sstream>

namespace mylang {

void SemanticAnalyzer::pushScope() { scopes.emplace_back(); }

void SemanticAnalyzer::popScope() { if (!scopes.empty()) scopes.pop_back(); }

bool SemanticAnalyzer::lookup(const std::string &name, Type &out) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto f = it->find(name);
        if (f != it->end()) { out = f->second; return true; }
    }
    return false;
}

bool SemanticAnalyzer::typesCompatible(Type a, Type b) const { return a == b; }

void SemanticAnalyzer::addDiagnostic(int line, int column, const std::string &msg) {
    std::ostringstream os;
    os << "[" << line << ":" << column << "] " << msg;
    diagnostics.push_back(os.str());
}

bool SemanticAnalyzer::analyze(const Program &program) {
    diagnostics.clear();
    pushScope();
    analyzeProgram(program);
    popScope();

    for (const auto &d : diagnostics) {
        std::cerr << d << '\n';
    }
    return diagnostics.empty();
}

void SemanticAnalyzer::analyzeProgram(const Program &program) {
    for (const auto &decl : program.decls) {
        if (auto fn = dynamic_cast<FunctionDecl*>(decl.get())) {
            analyzeFunction(*fn);
        }
    }
}

void SemanticAnalyzer::analyzeFunction(const FunctionDecl &fn) {
    pushScope();
    if (fn.body) analyzeStmt(fn.body.get(), fn.returnType);
    popScope();
}

void SemanticAnalyzer::analyzeStmt(const Stmt *stmt, Type expectedReturn) {
    if (auto block = dynamic_cast<const BlockStmt*>(stmt)) {
        pushScope();
        for (const auto &s : block->statements) analyzeStmt(s.get(), expectedReturn);
        popScope();
    } else if (auto decl = dynamic_cast<const VarDecl*>(stmt)) {
        Scope &scope = scopes.back();
        if (scope.count(decl->name)) {
            addDiagnostic(decl->line, decl->column, "redefinition of variable '" + decl->name + "'");
        } else {
            scope[decl->name] = decl->varType;
        }
        if (decl->init) {
            Type initType = analyzeExpr(decl->init.get());
            if (!typesCompatible(decl->varType, initType)) {
                addDiagnostic(decl->init->line, decl->init->column, "type mismatch in initialization of '" + decl->name + "'");
            }
        }
    } else if (auto ret = dynamic_cast<const ReturnStmt*>(stmt)) {
        Type valType = Type::Void;
        if (ret->value) valType = analyzeExpr(ret->value.get());
        if (!typesCompatible(expectedReturn, valType)) {
            addDiagnostic(ret->line, ret->column, "return type mismatch: expected " + std::string(typeToString(expectedReturn)));
        }
    } else if (auto exprStmt = dynamic_cast<const ExprStmt*>(stmt)) {
        if (exprStmt->expr) analyzeExpr(exprStmt->expr.get());
    }
}

Type SemanticAnalyzer::analyzeExpr(const Expr *expr) {
    if (auto lit = dynamic_cast<const Literal*>(expr)) {
        // crude literal type detection
        bool isNumber = true;
        for (char c : lit->value) if (!std::isdigit(c)) { isNumber = false; break; }
        return isNumber ? Type::Int : Type::String;
    } else if (auto id = dynamic_cast<const Identifier*>(expr)) {
        Type t{};
        if (!lookup(id->name, t)) {
            addDiagnostic(id->line, id->column, "use of undeclared identifier '" + id->name + "'");
            return Type::Int;
        }
        return t;
    } else if (auto bin = dynamic_cast<const BinaryExpr*>(expr)) {
        Type left = analyzeExpr(bin->left.get());
        Type right = analyzeExpr(bin->right.get());
        if (!typesCompatible(left, right)) {
            addDiagnostic(bin->line, bin->column, "type mismatch in binary expression");
        }
        return left;
    }
    return Type::Int;
}

} // namespace mylang
