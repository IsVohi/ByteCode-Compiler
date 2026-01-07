#include "ast.h"

// ============================================================================
// Expression Node Implementations
// ============================================================================

void NumberExpr::accept(ASTVisitor &visitor) const {
  visitor.visitNumberExpr(*this);
}

void IdentifierExpr::accept(ASTVisitor &visitor) const {
  visitor.visitIdentifierExpr(*this);
}

void BinaryOpExpr::accept(ASTVisitor &visitor) const {
  visitor.visitBinaryOpExpr(*this);
}

void UnaryOpExpr::accept(ASTVisitor &visitor) const {
  visitor.visitUnaryOpExpr(*this);
}

void FunctionCallExpr::accept(ASTVisitor &visitor) const {
  visitor.visitFunctionCallExpr(*this);
}

void StringLiteralExpr::accept(ASTVisitor &visitor) const {
  visitor.visitStringLiteralExpr(*this);
}

// ============================================================================
// Statement Node Implementations
// ============================================================================

void AssignmentStmt::accept(ASTVisitor &visitor) const {
  visitor.visitAssignmentStmt(*this);
}

void PrintStmt::accept(ASTVisitor &visitor) const {
  visitor.visitPrintStmt(*this);
}

void IfStmt::accept(ASTVisitor &visitor) const { visitor.visitIfStmt(*this); }

void WhileStmt::accept(ASTVisitor &visitor) const {
  visitor.visitWhileStmt(*this);
}

void ExpressionStmt::accept(ASTVisitor &visitor) const {
  visitor.visitExpressionStmt(*this);
}

void ReturnStmt::accept(ASTVisitor &visitor) const {
  visitor.visitReturnStmt(*this);
}

void BlockStmt::accept(ASTVisitor &visitor) const {
  visitor.visitBlockStmt(*this);
}

// ============================================================================
// Top-Level Node Implementations
// ============================================================================

void FunctionDecl::accept(ASTVisitor &visitor) const {
  visitor.visitFunctionDecl(*this);
}

void Program::accept(ASTVisitor &visitor) const { visitor.visitProgram(*this); }

void ForStmt::accept(ASTVisitor &visitor) const { visitor.visitForStmt(*this); }
void BreakStmt::accept(ASTVisitor &visitor) const {
  visitor.visitBreakStmt(*this);
}
void ContinueStmt::accept(ASTVisitor &visitor) const {
  visitor.visitContinueStmt(*this);
}

// Array Implementations
void ArrayLiteralExpr::accept(ASTVisitor &visitor) const {
  visitor.visitArrayLiteralExpr(*this);
}

void IndexExpr::accept(ASTVisitor &visitor) const {
  visitor.visitIndexExpr(*this);
}

void ArrayAssignmentStmt::accept(ASTVisitor &visitor) const {
  visitor.visitArrayAssignmentStmt(*this);
}
