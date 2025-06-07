#include "ast.hpp"
#include <iostream>

namespace mylang {

static void indent(int level) {
    for (int i = 0; i < level; ++i) std::cout << "  ";
}

void Program::dump(int indentLevel) const {
    indent(indentLevel);
    std::cout << "Program\n";
    for (const auto &fn : functions) {
        fn->dump(indentLevel + 1);
    }
}

void FunctionDecl::dump(int indentLevel) const {
    indent(indentLevel);
    std::cout << "FunctionDecl " << name << "\n";
    if (body) body->dump(indentLevel + 1);
}

void BlockStmt::dump(int indentLevel) const {
    indent(indentLevel);
    std::cout << "BlockStmt\n";
    for (const auto &stmt : statements) {
        stmt->dump(indentLevel + 1);
    }
}

void VarDecl::dump(int indentLevel) const {
    indent(indentLevel);
    std::cout << "VarDecl " << name << "\n";
    if (init) init->dump(indentLevel + 1);
}

void ReturnStmt::dump(int indentLevel) const {
    indent(indentLevel);
    std::cout << "ReturnStmt\n";
    if (value) value->dump(indentLevel + 1);
}

void ExprStmt::dump(int indentLevel) const {
    if (expr) expr->dump(indentLevel);
}

void BinaryExpr::dump(int indentLevel) const {
    indent(indentLevel);
    std::cout << "BinaryExpr '" << op << "'\n";
    if (left) left->dump(indentLevel + 1);
    if (right) right->dump(indentLevel + 1);
}

void Identifier::dump(int indentLevel) const {
    indent(indentLevel);
    std::cout << "Identifier " << name << "\n";
}

void Literal::dump(int indentLevel) const {
    indent(indentLevel);
    std::cout << "Literal " << value << "\n";
}

} // namespace mylang
