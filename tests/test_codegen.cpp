#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include <gtest/gtest.h>

class CodeGenTest : public ::testing::Test {
protected:
  BytecodeProgram compile(const std::string &source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parseProgram();
    CodeGenerator codegen;
    return codegen.generate(*program);
  }
};

// ============================================================================
// Basic Expression Tests
// ============================================================================

TEST_F(CodeGenTest, NumberExprGeneratesConst) {
  auto bytecode = compile("print(42);");

  // Should have at least CONST and PRINT instructions
  ASSERT_GE(bytecode.code.size(), static_cast<size_t>(2));

  // Check constant pool contains 42
  bool found42 = false;
  for (const auto &c : bytecode.constants) {
    if (c.isInt() && c.asInt() == 42)
      found42 = true;
  }
  EXPECT_TRUE(found42);
}

TEST_F(CodeGenTest, BinaryAddGeneratesAddOpcode) {
  auto bytecode = compile("print(3 + 5);");

  // Should have CONST, CONST, ADD, PRINT sequence
  bool hasAdd = false;
  for (const auto &instr : bytecode.code) {
    if (static_cast<Opcode>(instr.opcode) == Opcode::ADD) {
      hasAdd = true;
      break;
    }
  }
  EXPECT_TRUE(hasAdd);
}

TEST_F(CodeGenTest, VariableAssignmentGeneratesStore) {
  auto bytecode = compile("let x = 5;");

  // Should have CONST and STORE
  bool hasStore = false;
  for (const auto &instr : bytecode.code) {
    if (static_cast<Opcode>(instr.opcode) == Opcode::STORE) {
      hasStore = true;
      break;
    }
  }
  EXPECT_TRUE(hasStore);
}

TEST_F(CodeGenTest, VariableLoadGeneratesLoad) {
  auto bytecode = compile("let x = 5; print(x);");

  // Should have LOAD instruction
  bool hasLoad = false;
  for (const auto &instr : bytecode.code) {
    if (static_cast<Opcode>(instr.opcode) == Opcode::LOAD) {
      hasLoad = true;
      break;
    }
  }
  EXPECT_TRUE(hasLoad);
}

// ============================================================================
// Control Flow Tests
// ============================================================================

TEST_F(CodeGenTest, IfStatementGeneratesJump) {
  auto bytecode = compile("if (1) { print(42); }");

  // Should have JUMP_IF_ZERO
  bool hasJumpIfZero = false;
  for (const auto &instr : bytecode.code) {
    if (static_cast<Opcode>(instr.opcode) == Opcode::JUMP_IF_ZERO) {
      hasJumpIfZero = true;
      break;
    }
  }
  EXPECT_TRUE(hasJumpIfZero);
}

TEST_F(CodeGenTest, WhileLoopGeneratesJumps) {
  auto bytecode = compile("let i = 0; while (i) { i = 0; }");

  // Should have JUMP and JUMP_IF_ZERO
  bool hasJump = false;
  bool hasJumpIfZero = false;
  for (const auto &instr : bytecode.code) {
    if (static_cast<Opcode>(instr.opcode) == Opcode::JUMP) {
      hasJump = true;
    }
    if (static_cast<Opcode>(instr.opcode) == Opcode::JUMP_IF_ZERO) {
      hasJumpIfZero = true;
    }
  }
  EXPECT_TRUE(hasJump);
  EXPECT_TRUE(hasJumpIfZero);
}

// ============================================================================
// Function Tests
// ============================================================================

TEST_F(CodeGenTest, FunctionDeclCreatesEntry) {
  auto bytecode = compile("fn add(a, b) { return a + b; }");

  ASSERT_EQ(bytecode.functions.size(), static_cast<size_t>(1));
  EXPECT_EQ(bytecode.functions[0].name, "add");
  EXPECT_EQ(bytecode.functions[0].arity, 2);
}

TEST_F(CodeGenTest, FunctionCallGeneratesCall) {
  auto bytecode = compile("fn foo() { return 1; } print(foo());");

  bool hasCall = false;
  for (const auto &instr : bytecode.code) {
    if (static_cast<Opcode>(instr.opcode) == Opcode::CALL) {
      hasCall = true;
      break;
    }
  }
  EXPECT_TRUE(hasCall);
}

TEST_F(CodeGenTest, ReturnGeneratesReturnOpcode) {
  auto bytecode = compile("fn foo() { return 42; }");

  bool hasReturn = false;
  for (const auto &instr : bytecode.code) {
    if (static_cast<Opcode>(instr.opcode) == Opcode::RETURN) {
      hasReturn = true;
      break;
    }
  }
  EXPECT_TRUE(hasReturn);
}
