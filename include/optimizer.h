#ifndef COMPILER_OPTIMIZER_H
#define COMPILER_OPTIMIZER_H

#include "ast.h"
#include <memory>
#include <unordered_map>
#include <unordered_set>

/**
 * AST Optimizer implementing multiple optimization passes:
 * - Constant Folding: Evaluate constant expressions at compile time
 * - Dead Code Elimination: Remove unused assignments
 * - Function Inlining: Inline small non-recursive functions
 */
class Optimizer {
public:
  struct Stats {
    int constantsFolded = 0;
    int deadCodeRemoved = 0;
    int functionsInlined = 0;
  };

  Optimizer() = default;

  /**
   * Run all optimization passes on the program
   * @param program The program AST (modified in place)
   */
  void run(Program &program);

  /**
   * Run only constant folding pass
   */
  void runConstantFolding(Program &program);

  /**
   * Run only dead code elimination
   */
  void runDeadCodeElimination(Program &program);

  /**
   * Run only function inlining
   */
  void runFunctionInlining(Program &program);

  /**
   * Get optimization statistics
   */
  const Stats &getStats() const { return stats_; }

  /**
   * Reset statistics
   */
  void resetStats() { stats_ = Stats{}; }

private:
  Stats stats_;

  // Function map for inlining
  std::unordered_map<std::string, const FunctionDecl *> functionMap_;

  // Constant folding helpers
  void countFoldingOpportunities(const Stmt *stmt);
  std::unique_ptr<Expr> foldExpression(std::unique_ptr<Expr> expr);
  std::unique_ptr<Expr> foldBinaryOp(const BinaryOpExpr &expr);
  std::unique_ptr<Expr> foldUnaryOp(const UnaryOpExpr &expr);
  bool isConstant(const Expr &expr) const;
  int getConstantValue(const Expr &expr) const;

  // Dead code elimination helpers
  std::unordered_set<std::string>
  findUsedVariables(const std::vector<std::unique_ptr<Stmt>> &stmts);
  void collectUsedVars(const Stmt *stmt, std::unordered_set<std::string> &used);
  void collectUsedVarsFromExpr(const Expr *expr,
                               std::unordered_set<std::string> &used);
  void countDeadCode(const std::vector<std::unique_ptr<Stmt>> &stmts,
                     const std::unordered_set<std::string> &used);
  void eliminateDeadCode(std::vector<std::unique_ptr<Stmt>> &stmts);

  // Function inlining helpers
  bool canInline(const FunctionDecl &fn) const;
  bool containsCall(const Stmt *stmt, const std::string &fnName) const;
  bool containsCallExpr(const Expr *expr, const std::string &fnName) const;
  int countAstNodes(const FunctionDecl &fn) const;
  int countStmtNodes(const Stmt *stmt) const;
  int countExprNodes(const Expr *expr) const;
};

#endif // COMPILER_OPTIMIZER_H
