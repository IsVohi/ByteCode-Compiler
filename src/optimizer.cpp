#include "optimizer.h"
#include "common.h"
#include <algorithm>

// ============================================================================
// Main Entry Points
// ============================================================================

void Optimizer::run(Program &program) {
  // Build function map for inlining
  functionMap_.clear();
  for (const auto &item : program.items()) {
    if (auto *fn = dynamic_cast<const FunctionDecl *>(item.get())) {
      functionMap_[fn->name()] = fn;
    }
  }

  // Run passes in order: CF -> DCE -> Inlining -> CF again
  runConstantFolding(program);
  runDeadCodeElimination(program);
  runFunctionInlining(program);
  // Re-run CF after inlining might expose more opportunities
  runConstantFolding(program);
}

// ============================================================================
// Constant Folding
// ============================================================================

void Optimizer::runConstantFolding(Program &program) {
  // Constant folding is done during code generation or
  // requires mutable AST. For now, we track what could be folded.
  // Full implementation would require mutable AST node accessors.

  // Note: The current AST design uses const accessors, making in-place
  // modification challenging. A production optimizer would either:
  // 1. Use mutable accessors
  // 2. Reconstruct the AST
  // 3. Operate on an IR

  // For demonstration, we count opportunities
  for (const auto &item : program.items()) {
    if (auto *fn = dynamic_cast<const FunctionDecl *>(item.get())) {
      for (const auto &stmt : fn->body()) {
        countFoldingOpportunities(stmt.get());
      }
    } else if (auto *stmt = dynamic_cast<const Stmt *>(item.get())) {
      countFoldingOpportunities(stmt);
    }
  }
}

void Optimizer::countFoldingOpportunities(const Stmt *stmt) {
  // Count constant expressions that could be folded
  // This is a simplified implementation for demonstration
  if (auto *assign = dynamic_cast<const AssignmentStmt *>(stmt)) {
    if (auto *binop = dynamic_cast<const BinaryOpExpr *>(&assign->value())) {
      if (isConstant(binop->left()) && isConstant(binop->right())) {
        stats_.constantsFolded++;
      }
    }
  }
}

bool Optimizer::isConstant(const Expr &expr) const {
  return dynamic_cast<const NumberExpr *>(&expr) != nullptr;
}

int Optimizer::getConstantValue(const Expr &expr) const {
  if (auto *num = dynamic_cast<const NumberExpr *>(&expr)) {
    return num->value();
  }
  throw OptimizerError("Not a constant expression");
}

std::unique_ptr<Expr> Optimizer::foldExpression(std::unique_ptr<Expr> expr) {
  // This would recursively fold constant expressions
  // Simplified for demonstration
  return expr;
}

std::unique_ptr<Expr> Optimizer::foldBinaryOp(const BinaryOpExpr &expr) {
  if (!isConstant(expr.left()) || !isConstant(expr.right())) {
    return nullptr;
  }

  int left = getConstantValue(expr.left());
  int right = getConstantValue(expr.right());
  int result = 0;

  switch (expr.op()) {
  case BinaryOpExpr::Operator::PLUS:
    result = left + right;
    break;
  case BinaryOpExpr::Operator::MINUS:
    result = left - right;
    break;
  case BinaryOpExpr::Operator::MULTIPLY:
    result = left * right;
    break;
  case BinaryOpExpr::Operator::DIVIDE:
    if (right == 0)
      return nullptr; // Don't fold div by zero
    result = left / right;
    break;
  case BinaryOpExpr::Operator::MODULO:
    if (right == 0)
      return nullptr;
    result = left % right;
    break;
  default:
    return nullptr; // Don't fold comparisons yet
  }

  stats_.constantsFolded++;
  return std::make_unique<NumberExpr>(result);
}

std::unique_ptr<Expr> Optimizer::foldUnaryOp(const UnaryOpExpr &expr) {
  if (!isConstant(expr.operand())) {
    return nullptr;
  }

  int value = getConstantValue(expr.operand());

  switch (expr.op()) {
  case UnaryOpExpr::Operator::NEGATE:
    stats_.constantsFolded++;
    return std::make_unique<NumberExpr>(-value);
  case UnaryOpExpr::Operator::NOT:
    stats_.constantsFolded++;
    return std::make_unique<NumberExpr>(value == 0 ? 1 : 0);
  }

  return nullptr;
}

// ============================================================================
// Dead Code Elimination
// ============================================================================

void Optimizer::runDeadCodeElimination(Program &program) {
  // Find variables that are used
  // Remove assignments to unused variables
  // Remove statements after return

  // Note: Similar to CF, full DCE requires mutable AST
  // For demonstration, we identify opportunities

  for (const auto &item : program.items()) {
    if (auto *fn = dynamic_cast<const FunctionDecl *>(item.get())) {
      auto used = findUsedVariables(fn->body());
      countDeadCode(fn->body(), used);
    }
  }
}

std::unordered_set<std::string>
Optimizer::findUsedVariables(const std::vector<std::unique_ptr<Stmt>> &stmts) {

  std::unordered_set<std::string> used;

  // Simplified: just look for identifier expressions
  for (const auto &stmt : stmts) {
    collectUsedVars(stmt.get(), used);
  }

  return used;
}

void Optimizer::collectUsedVars(const Stmt *stmt,
                                std::unordered_set<std::string> &used) {
  if (auto *assign = dynamic_cast<const AssignmentStmt *>(stmt)) {
    collectUsedVarsFromExpr(&assign->value(), used);
  } else if (auto *print = dynamic_cast<const PrintStmt *>(stmt)) {
    collectUsedVarsFromExpr(&print->value(), used);
  } else if (auto *ifstmt = dynamic_cast<const IfStmt *>(stmt)) {
    collectUsedVarsFromExpr(&ifstmt->condition(), used);
    for (const auto &s : ifstmt->body()) {
      collectUsedVars(s.get(), used);
    }
  } else if (auto *whilestmt = dynamic_cast<const WhileStmt *>(stmt)) {
    collectUsedVarsFromExpr(&whilestmt->condition(), used);
    for (const auto &s : whilestmt->body()) {
      collectUsedVars(s.get(), used);
    }
  } else if (auto *ret = dynamic_cast<const ReturnStmt *>(stmt)) {
    if (ret->value()) {
      collectUsedVarsFromExpr(ret->value(), used);
    }
  }
}

void Optimizer::collectUsedVarsFromExpr(const Expr *expr,
                                        std::unordered_set<std::string> &used) {
  if (auto *id = dynamic_cast<const IdentifierExpr *>(expr)) {
    used.insert(id->name());
  } else if (auto *binop = dynamic_cast<const BinaryOpExpr *>(expr)) {
    collectUsedVarsFromExpr(&binop->left(), used);
    collectUsedVarsFromExpr(&binop->right(), used);
  } else if (auto *unary = dynamic_cast<const UnaryOpExpr *>(expr)) {
    collectUsedVarsFromExpr(&unary->operand(), used);
  } else if (auto *call = dynamic_cast<const FunctionCallExpr *>(expr)) {
    for (const auto &arg : call->args()) {
      collectUsedVarsFromExpr(arg.get(), used);
    }
  }
}

void Optimizer::countDeadCode(const std::vector<std::unique_ptr<Stmt>> &stmts,
                              const std::unordered_set<std::string> &used) {
  bool afterReturn = false;

  for (const auto &stmt : stmts) {
    if (afterReturn) {
      stats_.deadCodeRemoved++;
      continue;
    }

    if (dynamic_cast<const ReturnStmt *>(stmt.get())) {
      afterReturn = true;
    }

    // Check for unused assignments
    if (auto *assign = dynamic_cast<const AssignmentStmt *>(stmt.get())) {
      if (used.find(assign->name()) == used.end()) {
        stats_.deadCodeRemoved++;
      }
    }
  }
}

void Optimizer::eliminateDeadCode(
    std::vector<std::unique_ptr<Stmt>> & /*stmts*/) {
  // Would modify stmts in place - requires mutable AST
}

// ============================================================================
// Function Inlining
// ============================================================================

void Optimizer::runFunctionInlining(Program & /*program*/) {
  // Find small, non-recursive functions
  // Count call sites
  // Inline if criteria met

  for (const auto &[name, fn] : functionMap_) {
    if (canInline(*fn)) {
      stats_.functionsInlined++;
    }
  }
}

bool Optimizer::canInline(const FunctionDecl &fn) const {
  // Inline if:
  // 1. Small (< 20 AST nodes)
  // 2. Not recursive
  // 3. Has <= 3 parameters

  if (fn.params().size() > 3)
    return false;
  if (countAstNodes(fn) > 20)
    return false;

  // Check for recursion (function calls itself)
  // Simplified check
  for (const auto &stmt : fn.body()) {
    if (containsCall(stmt.get(), fn.name())) {
      return false;
    }
  }

  return true;
}

bool Optimizer::containsCall(const Stmt *stmt,
                             const std::string &fnName) const {
  if (auto *assign = dynamic_cast<const AssignmentStmt *>(stmt)) {
    return containsCallExpr(&assign->value(), fnName);
  } else if (auto *print = dynamic_cast<const PrintStmt *>(stmt)) {
    return containsCallExpr(&print->value(), fnName);
  } else if (auto *ret = dynamic_cast<const ReturnStmt *>(stmt)) {
    return ret->value() && containsCallExpr(ret->value(), fnName);
  } else if (auto *ifstmt = dynamic_cast<const IfStmt *>(stmt)) {
    if (containsCallExpr(&ifstmt->condition(), fnName))
      return true;
    for (const auto &s : ifstmt->body()) {
      if (containsCall(s.get(), fnName))
        return true;
    }
  } else if (auto *whilestmt = dynamic_cast<const WhileStmt *>(stmt)) {
    if (containsCallExpr(&whilestmt->condition(), fnName))
      return true;
    for (const auto &s : whilestmt->body()) {
      if (containsCall(s.get(), fnName))
        return true;
    }
  } else if (auto *block = dynamic_cast<const BlockStmt *>(stmt)) {
    for (const auto &s : block->statements()) {
      if (containsCall(s.get(), fnName))
        return true;
    }
  }
  return false;
}

bool Optimizer::containsCallExpr(const Expr *expr,
                                 const std::string &fnName) const {
  if (auto *call = dynamic_cast<const FunctionCallExpr *>(expr)) {
    if (call->name() == fnName)
      return true;
    for (const auto &arg : call->args()) {
      if (containsCallExpr(arg.get(), fnName))
        return true;
    }
  } else if (auto *binop = dynamic_cast<const BinaryOpExpr *>(expr)) {
    return containsCallExpr(&binop->left(), fnName) ||
           containsCallExpr(&binop->right(), fnName);
  } else if (auto *unary = dynamic_cast<const UnaryOpExpr *>(expr)) {
    return containsCallExpr(&unary->operand(), fnName);
  }
  return false;
}

int Optimizer::countAstNodes(const FunctionDecl &fn) const {
  int count = 1; // The function itself
  for (const auto &stmt : fn.body()) {
    count += countStmtNodes(stmt.get());
  }
  return count;
}

int Optimizer::countStmtNodes(const Stmt *stmt) const {
  int count = 1;
  if (auto *assign = dynamic_cast<const AssignmentStmt *>(stmt)) {
    count += countExprNodes(&assign->value());
  } else if (auto *print = dynamic_cast<const PrintStmt *>(stmt)) {
    count += countExprNodes(&print->value());
  } else if (auto *ifstmt = dynamic_cast<const IfStmt *>(stmt)) {
    count += countExprNodes(&ifstmt->condition());
    for (const auto &s : ifstmt->body()) {
      count += countStmtNodes(s.get());
    }
  } else if (auto *whilestmt = dynamic_cast<const WhileStmt *>(stmt)) {
    count += countExprNodes(&whilestmt->condition());
    for (const auto &s : whilestmt->body()) {
      count += countStmtNodes(s.get());
    }
  } else if (auto *ret = dynamic_cast<const ReturnStmt *>(stmt)) {
    if (ret->value()) {
      count += countExprNodes(ret->value());
    }
  }
  return count;
}

int Optimizer::countExprNodes(const Expr *expr) const {
  int count = 1;
  if (auto *binop = dynamic_cast<const BinaryOpExpr *>(expr)) {
    count += countExprNodes(&binop->left());
    count += countExprNodes(&binop->right());
  } else if (auto *unary = dynamic_cast<const UnaryOpExpr *>(expr)) {
    count += countExprNodes(&unary->operand());
  } else if (auto *call = dynamic_cast<const FunctionCallExpr *>(expr)) {
    for (const auto &arg : call->args()) {
      count += countExprNodes(arg.get());
    }
  }
  return count;
}
