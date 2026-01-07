#ifndef COMPILER_AST_H
#define COMPILER_AST_H

#include <memory>
#include <string>
#include <vector>

class ASTVisitor;

class ASTNode {
public:
  virtual ~ASTNode() = default;

  virtual void accept(ASTVisitor &visitor) const = 0;
};

class Expr : public ASTNode {
public:
  ~Expr() override = default;
};

class Stmt : public ASTNode {
public:
  ~Stmt() override = default;
};

// Expression Nodes (6 types)
// ============================================================================
class NumberExpr : public Expr {
public:
  explicit NumberExpr(int value) : value_(value) {}

  void accept(ASTVisitor &visitor) const override;

  int value() const { return value_; }

private:
  int value_;
};

class IdentifierExpr : public Expr {
public:
  explicit IdentifierExpr(std::string name) : name_(std::move(name)) {}

  void accept(ASTVisitor &visitor) const override;

  const std::string &name() const { return name_; }

private:
  std::string name_;
};

class BinaryOpExpr : public Expr {
public:
  enum class Operator {
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    AND,
    OR
  };

  BinaryOpExpr(std::unique_ptr<Expr> left, Operator op,
               std::unique_ptr<Expr> right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}

  void accept(ASTVisitor &visitor) const override;

  const Expr &left() const { return *left_; }
  const Expr &right() const { return *right_; }
  Operator op() const { return op_; }

private:
  std::unique_ptr<Expr> left_;
  Operator op_;
  std::unique_ptr<Expr> right_;
};

class UnaryOpExpr : public Expr {
public:
  enum class Operator {
    NEGATE, // -expr
    NOT     // !expr
  };

  UnaryOpExpr(Operator op, std::unique_ptr<Expr> operand)
      : op_(op), operand_(std::move(operand)) {}

  void accept(ASTVisitor &visitor) const override;

  const Expr &operand() const { return *operand_; }
  Operator op() const { return op_; }

private:
  Operator op_;
  std::unique_ptr<Expr> operand_;
};

class FunctionCallExpr : public Expr {
public:
  FunctionCallExpr(std::string name, std::vector<std::unique_ptr<Expr>> args)
      : name_(std::move(name)), args_(std::move(args)) {}

  const std::string &name() const { return name_; }
  const std::vector<std::unique_ptr<Expr>> &args() const { return args_; }

  void accept(ASTVisitor &visitor) const override;

private:
  std::string name_;
  std::vector<std::unique_ptr<Expr>> args_;
};

/**
 * Expression node for array literals
 */
class ArrayLiteralExpr : public Expr {
public:
  explicit ArrayLiteralExpr(std::vector<std::unique_ptr<Expr>> elements)
      : elements_(std::move(elements)) {}

  const std::vector<std::unique_ptr<Expr>> &elements() const {
    return elements_;
  }

  void accept(ASTVisitor &visitor) const override;

private:
  std::vector<std::unique_ptr<Expr>> elements_;
};

/**
 * Expression node for array indexing
 */
class IndexExpr : public Expr {
public:
  IndexExpr(std::unique_ptr<Expr> target, std::unique_ptr<Expr> index)
      : target_(std::move(target)), index_(std::move(index)) {}

  const Expr &target() const { return *target_; }
  const Expr &index() const { return *index_; }

  std::unique_ptr<Expr> takeTarget() { return std::move(target_); }
  std::unique_ptr<Expr> takeIndex() { return std::move(index_); }

  void accept(ASTVisitor &visitor) const override;

private:
  std::unique_ptr<Expr> target_;
  std::unique_ptr<Expr> index_;
};

class StringLiteralExpr : public Expr {
public:
  explicit StringLiteralExpr(std::string value) : value_(std::move(value)) {}

  void accept(ASTVisitor &visitor) const override;

  const std::string &value() const { return value_; }

private:
  std::string value_;
};

// Statement Nodes (5 types)

/**
 * Statement node for variable assignment
 */
class AssignmentStmt : public Stmt {
public:
  AssignmentStmt(std::string name, std::unique_ptr<Expr> value)
      : name_(std::move(name)), value_(std::move(value)) {}

  const std::string &name() const { return name_; }
  const Expr &value() const { return *value_; }

  void accept(ASTVisitor &visitor) const override;

private:
  std::string name_;
  std::unique_ptr<Expr> value_;
};

/**
 * Statement node for array element assignment
 */
class ArrayAssignmentStmt : public Stmt {
public:
  ArrayAssignmentStmt(std::unique_ptr<Expr> target, std::unique_ptr<Expr> index,
                      std::unique_ptr<Expr> value)
      : target_(std::move(target)), index_(std::move(index)),
        value_(std::move(value)) {}

  const Expr &target() const { return *target_; }
  const Expr &index() const { return *index_; }
  const Expr &value() const { return *value_; }

  void accept(ASTVisitor &visitor) const override;

private:
  std::unique_ptr<Expr> target_;
  std::unique_ptr<Expr> index_;
  std::unique_ptr<Expr> value_;
};

class ExpressionStmt : public Stmt {
public:
  explicit ExpressionStmt(std::unique_ptr<Expr> expr)
      : expr_(std::move(expr)) {}

  void accept(ASTVisitor &visitor) const override;

  const Expr &expr() const { return *expr_; }

private:
  std::unique_ptr<Expr> expr_;
};

class PrintStmt : public Stmt {
public:
  explicit PrintStmt(std::unique_ptr<Expr> value) : value_(std::move(value)) {}

  void accept(ASTVisitor &visitor) const override;

  const Expr &value() const { return *value_; }

private:
  std::unique_ptr<Expr> value_;
};

class IfStmt : public Stmt {
public:
  IfStmt(std::unique_ptr<Expr> condition,
         std::vector<std::unique_ptr<Stmt>> body)
      : condition_(std::move(condition)), body_(std::move(body)) {}

  void accept(ASTVisitor &visitor) const override;

  const Expr &condition() const { return *condition_; }
  const std::vector<std::unique_ptr<Stmt>> &body() const { return body_; }

private:
  std::unique_ptr<Expr> condition_;
  std::vector<std::unique_ptr<Stmt>> body_;
};

class WhileStmt : public Stmt {
public:
  WhileStmt(std::unique_ptr<Expr> condition,
            std::vector<std::unique_ptr<Stmt>> body)
      : condition_(std::move(condition)), body_(std::move(body)) {}

  void accept(ASTVisitor &visitor) const override;

  const Expr &condition() const { return *condition_; }
  const std::vector<std::unique_ptr<Stmt>> &body() const { return body_; }

private:
  std::unique_ptr<Expr> condition_;
  std::vector<std::unique_ptr<Stmt>> body_;
};

class ForStmt : public Stmt {
public:
  ForStmt(std::unique_ptr<Stmt> init, std::unique_ptr<Expr> condition,
          std::unique_ptr<Stmt> increment,
          std::vector<std::unique_ptr<Stmt>> body)
      : init_(std::move(init)), condition_(std::move(condition)),
        increment_(std::move(increment)), body_(std::move(body)) {}

  void accept(ASTVisitor &visitor) const override;

  const Stmt *init() const { return init_.get(); }
  const Expr *condition() const { return condition_.get(); }
  const Stmt *increment() const { return increment_.get(); }
  const std::vector<std::unique_ptr<Stmt>> &body() const { return body_; }

private:
  std::unique_ptr<Stmt> init_;
  std::unique_ptr<Expr> condition_;
  std::unique_ptr<Stmt> increment_;
  std::vector<std::unique_ptr<Stmt>> body_;
};

class BreakStmt : public Stmt {
public:
  BreakStmt() = default;
  void accept(ASTVisitor &visitor) const override;
};

class ContinueStmt : public Stmt {
public:
  ContinueStmt() = default;
  void accept(ASTVisitor &visitor) const override;
};

/**
 * Represents a return statement
 */
class ReturnStmt : public Stmt {
public:
  explicit ReturnStmt(std::unique_ptr<Expr> value = nullptr)
      : value_(std::move(value)) {}

  void accept(ASTVisitor &visitor) const override;

  const Expr *value() const { return value_.get(); }

private:
  std::unique_ptr<Expr> value_;
};

/**
 * Represents a block of statements: { stmt* }
 * Creates a scope for variables.
 */
class BlockStmt : public Stmt {
public:
  explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
      : statements_(std::move(statements)) {}

  void accept(ASTVisitor &visitor) const override;

  const std::vector<std::unique_ptr<Stmt>> &statements() const {
    return statements_;
  }

private:
  std::vector<std::unique_ptr<Stmt>> statements_;
};

// ============================================================================
// Top-Level Nodes (2 types)
// ============================================================================

/**
 * Represents a function declaration: fn name(params) { body }
 * Examples: fn factorial(n) { return n * factorial(n - 1); }
 */
class FunctionDecl : public ASTNode {
public:
  FunctionDecl(std::string name, std::vector<std::string> params,
               std::vector<std::unique_ptr<Stmt>> body)
      : name_(std::move(name)), params_(std::move(params)),
        body_(std::move(body)) {}

  void accept(ASTVisitor &visitor) const override;

  const std::string &name() const { return name_; }
  const std::vector<std::string> &params() const { return params_; }
  const std::vector<std::unique_ptr<Stmt>> &body() const { return body_; }

private:
  std::string name_;
  std::vector<std::string> params_;
  std::vector<std::unique_ptr<Stmt>> body_;
};

/**
 * Represents the entire program: collection of function declarations and
 * statements. Root node of the AST.
 */
class Program : public ASTNode {
public:
  explicit Program(std::vector<std::unique_ptr<ASTNode>> items)
      : items_(std::move(items)) {}

  void accept(ASTVisitor &visitor) const override;

  const std::vector<std::unique_ptr<ASTNode>> &items() const { return items_; }

private:
  std::vector<std::unique_ptr<ASTNode>> items_;
};

// ============================================================================
// Visitor Pattern
// ============================================================================

/**
 * Abstract base class for AST visitors.
 * Implements the visitor pattern for traversing and operating on AST nodes.
 * Each concrete visitor implements these methods to perform different
 * operations:
 * - Code generation
 * - Type checking
 * - Optimization
 * - Pretty printing
 */
class ASTVisitor {
public:
  virtual ~ASTVisitor() = default;

  // Expression visitors
  virtual void visitNumberExpr(const NumberExpr &) = 0;
  virtual void visitIdentifierExpr(const IdentifierExpr &) = 0;
  virtual void visitBinaryOpExpr(const BinaryOpExpr &) = 0;
  virtual void visitUnaryOpExpr(const UnaryOpExpr &) = 0;
  virtual void visitStringLiteralExpr(const StringLiteralExpr &) = 0;
  virtual void visitFunctionCallExpr(const FunctionCallExpr &) = 0;
  virtual void visitArrayLiteralExpr(const ArrayLiteralExpr &) = 0;
  virtual void visitIndexExpr(const IndexExpr &) = 0;

  // Statement visitors
  virtual void visitAssignmentStmt(const AssignmentStmt &) = 0;
  virtual void visitArrayAssignmentStmt(const ArrayAssignmentStmt &) = 0;
  virtual void visitPrintStmt(const PrintStmt &) = 0;
  virtual void visitExpressionStmt(const ExpressionStmt &) = 0;
  virtual void visitIfStmt(const IfStmt &) = 0;
  virtual void visitWhileStmt(const WhileStmt &) = 0;
  virtual void visitForStmt(const ForStmt &) = 0;
  virtual void visitBreakStmt(const BreakStmt &) = 0;
  virtual void visitContinueStmt(const ContinueStmt &) = 0;
  virtual void visitReturnStmt(const ReturnStmt &) = 0;
  virtual void visitBlockStmt(const BlockStmt &) = 0;

  // Top-level visitors
  virtual void visitFunctionDecl(const FunctionDecl &) = 0;
  virtual void visitProgram(const Program &) = 0;
};

#endif // COMPILER_AST_H
