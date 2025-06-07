#include "semantic.hpp"
#include <iostream>

namespace mylang {

void SemanticAnalyzer::pushScope() { scopes.emplace_back(); }
void SemanticAnalyzer::popScope() { scopes.pop_back(); }

void SemanticAnalyzer::error(const std::string &msg) {
    std::cerr << "Semantic error: " << msg << "\n";
    hasError = true;
}

Type SemanticAnalyzer::lookup(const std::string &name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return found->second;
    }
    error("undeclared identifier " + name);
    return Type::Int;
}

void SemanticAnalyzer::visitFunction(FunctionDecl &fn) {
    currentReturn = fn.returnType;
    pushScope();
    if (auto *body = dynamic_cast<BlockStmt*>(fn.body.get())) {
        visitBlock(*body);
    }
    popScope();
}

void SemanticAnalyzer::visitBlock(BlockStmt &block) {
    pushScope();
    for (auto &stmt : block.statements) {
        visitStmt(*stmt);
    }
    popScope();
}

void SemanticAnalyzer::visitStmt(Stmt &stmt) {
    if (auto *vd = dynamic_cast<VarDecl*>(&stmt)) {
        auto &scope = scopes.back();
        if (scope.count(vd->name)) {
            error("redeclared variable " + vd->name);
        } else {
            scope[vd->name] = vd->varType;
        }
        if (vd->init) {
            Type initTy = visitExpr(*vd->init);
            if (initTy != vd->varType) {
                error("type mismatch in initialization of " + vd->name);
            }
        }
    } else if (auto *ret = dynamic_cast<ReturnStmt*>(&stmt)) {
        if (ret->value) {
            Type valTy = visitExpr(*ret->value);
            if (valTy != currentReturn) {
                error("return type mismatch");
            }
        } else if (currentReturn != Type::Void) {
            error("non-void function must return a value");
        }
    } else if (auto *es = dynamic_cast<ExprStmt*>(&stmt)) {
        if (es->expr) visitExpr(*es->expr);
    } else if (auto *blk = dynamic_cast<BlockStmt*>(&stmt)) {
        visitBlock(*blk);
    }
}

Type SemanticAnalyzer::visitExpr(Expr &expr) {
    if (auto *id = dynamic_cast<Identifier*>(&expr)) {
        return lookup(id->name);
    }
    if (auto *lit = dynamic_cast<Literal*>(&expr)) {
        return lit->litType;
    }
    if (auto *bin = dynamic_cast<BinaryExpr*>(&expr)) {
        Type lt = visitExpr(*bin->left);
        Type rt = visitExpr(*bin->right);
        if (lt != rt) {
            error("type mismatch in binary expression");
        }
        return lt;
    }
    return Type::Int;
}

bool SemanticAnalyzer::analyze(Program &program) {
    pushScope();
    for (auto &decl : program.decls) {
        if (auto *fn = dynamic_cast<FunctionDecl*>(decl.get())) {
            visitFunction(*fn);
        }
    }
    popScope();
    return !hasError;
}

} // namespace mylang
