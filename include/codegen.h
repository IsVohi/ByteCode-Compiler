#ifndef COMPILER_CODEGEN_H
#define COMPILER_CODEGEN_H

#include "ast.h"
#include "common.h"
#include <cstdint>
#include <string>
#include <unordered_map>

#include <vector>

// ============================================================================
// Bytecode Program Structures
// ============================================================================

/**
 * Holds metadata for a compiled function
 */
struct FunctionInfo {
  std::string name;   // Function name
  uint16_t entry;     // Entry point (instruction index)
  uint8_t arity;      // Number of parameters
  uint8_t localCount; // Number of local variables
};

/**
 * Represents a complete compiled bytecode program
 */
struct BytecodeProgram {
  std::vector<Instruction> code;       // Bytecode instructions
  std::vector<Value> constants;        // Constant pool
  std::vector<FunctionInfo> functions; // Function metadata
  uint16_t mainEntry = 0;              // Entry point for main code

  /**
   * Dump the bytecode to stdout for debugging
   */
  void dump() const;
};

// ============================================================================
// Code Generator
// ============================================================================

/**
 * Generates bytecode from an AST using the visitor pattern.
 * Traverses the AST and emits stack-based bytecode instructions.
 */
class CodeGenerator : public ASTVisitor {
public:
  CodeGenerator() {
    scopes_.emplace_back(); // Initialize global scope
  }

  /**
   * Generate bytecode from a parsed program
   * @param program The AST root node
   * @return The compiled bytecode program
   */
  BytecodeProgram generate(const Program &program, bool incremental = false);

  // Expression visitors - generate code that pushes result on stack
  void visitNumberExpr(const NumberExpr &expr) override;
  void visitStringLiteralExpr(const StringLiteralExpr &expr) override;
  void visitIdentifierExpr(const IdentifierExpr &expr) override;
  void visitBinaryOpExpr(const BinaryOpExpr &expr) override;
  void visitUnaryOpExpr(const UnaryOpExpr &expr) override;
  void visitFunctionCallExpr(const FunctionCallExpr &expr) override;
  void visitArrayLiteralExpr(const ArrayLiteralExpr &expr) override;
  void visitIndexExpr(const IndexExpr &expr) override;

  // Statement visitors - generate code for side effects
  void visitAssignmentStmt(const AssignmentStmt &stmt) override;
  void visitArrayAssignmentStmt(const ArrayAssignmentStmt &stmt) override;
  void visitPrintStmt(const PrintStmt &stmt) override;
  void visitExpressionStmt(const ExpressionStmt &stmt) override;
  void visitIfStmt(const IfStmt &stmt) override;
  void visitWhileStmt(const WhileStmt &stmt) override;
  void visitForStmt(const ForStmt &stmt) override;
  void visitBreakStmt(const BreakStmt &stmt) override;
  void visitContinueStmt(const ContinueStmt &stmt) override;
  void visitReturnStmt(const ReturnStmt &stmt) override;
  void visitBlockStmt(const BlockStmt &stmt) override;

  // Top-level visitors
  void visitFunctionDecl(const FunctionDecl &decl) override;
  void visitProgram(const Program &program) override;

private:
  BytecodeProgram program_;

  // Current function being compiled
  std::string currentFunction_;

  // Symbol tables
  std::vector<std::unordered_map<std::string, uint16_t>>
      scopes_;                                        // Stack of local scopes
  std::unordered_map<std::string, uint16_t> globals_; // Global variable indices

  // Function lookup (name -> index in functions vector)
  std::unordered_map<std::string, uint16_t> functionMap_;

  // Loop handling
  struct LoopContext {
    int continueTarget; // Target IP for continue (or -1 if needs patching)
    std::vector<size_t> breakJumps;    // Offsets to patch for break
    std::vector<size_t> continueJumps; // Offsets to patch for continue
  };
  std::vector<LoopContext> loopStack_;

  // Helpers

  /**
   * Emit an instruction and return its index
   */
  uint16_t emit(Opcode op, uint16_t operand = 0);

  size_t emitJump(Opcode op);

  /**
   * Add a constant to the pool and return its index
   */
  uint16_t addConstant(Value value);

  /**
   * Get or create a variable slot
   */
  uint16_t getOrCreateLocal(const std::string &name);

  /**
   * Get a variable slot (throws if not found)
   */
  uint16_t getLocal(const std::string &name) const;

  /**
   * Check if in global scope (not inside a function)
   */
  bool isGlobalScope() const;

  /**
   * Patch a jump instruction with a target address
   */
  void patchJump(size_t jumpIndex, uint16_t target);

  /**
   * Get current instruction index
   */
  uint16_t currentIndex() const;

  /**
   * Reset state for compiling a new function
   */
  void beginFunction(const std::string &name,
                     const std::vector<std::string> &params);

  /**
   * Finalize function compilation
   */
  void endFunction();
};

#endif // COMPILER_CODEGEN_H
