#include "codegen.h"
#include "lexer.h"
#include "optimizer.h"
#include "parser.h"
#include "vm.h"
#include <gtest/gtest.h>
#include <sstream>

class EndToEndTest : public ::testing::Test {
protected:
  VirtualMachine vm;
  std::stringstream output;

  void SetUp() override { vm.setOutputStream(output); }

  Value run(const std::string &source, bool optimize = true) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parseProgram();

    if (optimize) {
      Optimizer optimizer;
      optimizer.run(*program);
    }

    CodeGenerator codegen;
    auto bytecode = codegen.generate(*program);

    return vm.execute(bytecode);
  }

  std::vector<Value> getOutput() { return vm.getOutput(); }
};

// ============================================================================
// Basic Expression Tests
// ============================================================================

TEST_F(EndToEndTest, SimpleAddition) {
  run("print(3 + 5);");
  auto out = getOutput();
  ASSERT_EQ(out.size(), static_cast<size_t>(1));
  EXPECT_EQ(out[0], 8);
}

TEST_F(EndToEndTest, ArithmeticExpression) {
  run("print(2 + 3 * 4);");
  auto out = getOutput();
  EXPECT_EQ(out[0], 14); // 2 + (3 * 4) = 14
}

TEST_F(EndToEndTest, ParenthesizedExpression) {
  run("print((2 + 3) * 4);");
  auto out = getOutput();
  EXPECT_EQ(out[0], 20); // (2 + 3) * 4 = 20
}

// ============================================================================
// Variable Tests
// ============================================================================

TEST_F(EndToEndTest, VariableAssignmentAndUse) {
  run("let x = 42; print(x);");
  auto out = getOutput();
  EXPECT_EQ(out[0], 42);
}

TEST_F(EndToEndTest, MultipleVariables) {
  run("let a = 10; let b = 20; print(a + b);");
  auto out = getOutput();
  EXPECT_EQ(out[0], 30);
}

TEST_F(EndToEndTest, VariableReassignment) {
  run("let x = 5; x = x + 1; print(x);");
  auto out = getOutput();
  EXPECT_EQ(out[0], 6);
}

// ============================================================================
// Control Flow Tests
// ============================================================================

TEST_F(EndToEndTest, IfStatementTrue) {
  run("if (1) { print(42); }");
  auto out = getOutput();
  ASSERT_EQ(out.size(), static_cast<size_t>(1));
  EXPECT_EQ(out[0], 42);
}

TEST_F(EndToEndTest, IfStatementFalse) {
  run("if (0) { print(42); }");
  auto out = getOutput();
  EXPECT_EQ(out.size(), static_cast<size_t>(0));
}

TEST_F(EndToEndTest, WhileLoop) {
  run("let i = 0; while (i) { print(i); i = i - 1; }");
  // i starts at 0, so loop body never executes
  auto out = getOutput();
  EXPECT_EQ(out.size(), static_cast<size_t>(0));
}

// ============================================================================
// Function Tests
// ============================================================================

TEST_F(EndToEndTest, SimpleFunctionCall) {
  run("fn double(x) { return x + x; } print(double(21));");
  auto out = getOutput();
  EXPECT_EQ(out[0], 42);
}

TEST_F(EndToEndTest, FunctionWithMultipleParams) {
  run("fn add(a, b) { return a + b; } print(add(17, 25));");
  auto out = getOutput();
  EXPECT_EQ(out[0], 42);
}

TEST_F(EndToEndTest, NestedFunctionCalls) {
  run("fn double(x) { return x + x; } fn quad(x) { return double(double(x)); } "
      "print(quad(10));");
  auto out = getOutput();
  EXPECT_EQ(out[0], 40);
}

// ============================================================================
// Optimization Comparison Tests
// ============================================================================

TEST_F(EndToEndTest, OptimizedAndUnoptimizedSameResult) {
  std::string source = "let x = 2 + 3; let y = x * 2; print(y);";

  Value optimized = run(source, true);
  Value unoptimized = run(source, false);

  // Results should be the same
  auto outOpt = getOutput();
  // Note: getOutput() returns cumulative, so we check last values
  EXPECT_EQ(optimized, unoptimized);
}
