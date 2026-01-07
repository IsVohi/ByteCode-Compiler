#include "lexer.h"
#include "optimizer.h"
#include "parser.h"
#include <gtest/gtest.h>

class OptimizerTest : public ::testing::Test {
protected:
  std::unique_ptr<Program> parse(const std::string &source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parseProgram();
  }
};

// ============================================================================
// Constant Folding Tests
// ============================================================================

TEST_F(OptimizerTest, DetectsConstantFoldingOpportunity) {
  auto program = parse("let x = 2 + 3;");

  Optimizer optimizer;
  optimizer.run(*program);

  auto stats = optimizer.getStats();
  EXPECT_GE(stats.constantsFolded, 1);
}

TEST_F(OptimizerTest, MultipleFoldingOpportunities) {
  auto program = parse("let x = 2 + 3; let y = 10 * 5;");

  Optimizer optimizer;
  optimizer.run(*program);

  auto stats = optimizer.getStats();
  EXPECT_GE(stats.constantsFolded, 2);
}

// ============================================================================
// Dead Code Elimination Tests
// ============================================================================

TEST_F(OptimizerTest, DetectsUnusedVariable) {
  auto program =
      parse("fn foo() { let unused = 5; let used = 10; return used; }");

  Optimizer optimizer;
  optimizer.run(*program);

  auto stats = optimizer.getStats();
  EXPECT_GE(stats.deadCodeRemoved, 1);
}

TEST_F(OptimizerTest, DetectsCodeAfterReturn) {
  auto program = parse("fn foo() { return 1; let dead = 5; }");

  Optimizer optimizer;
  optimizer.run(*program);

  auto stats = optimizer.getStats();
  EXPECT_GE(stats.deadCodeRemoved, 1);
}

// ============================================================================
// Function Inlining Tests
// ============================================================================

TEST_F(OptimizerTest, IdentifiesInlinableFunction) {
  auto program = parse("fn add(a, b) { return a + b; }");

  Optimizer optimizer;
  optimizer.run(*program);

  auto stats = optimizer.getStats();
  // Small function should be inlinable
  EXPECT_GE(stats.functionsInlined, 1);
}

TEST_F(OptimizerTest, DoesNotInlineRecursiveFunction) {
  auto program = parse(
      "fn factorial(n) { if (n) { return n * factorial(n - 1); } return 1; }");

  Optimizer optimizer;
  optimizer.run(*program);

  auto stats = optimizer.getStats();
  // Recursive functions should not be inlined
  EXPECT_EQ(stats.functionsInlined, 0);
}

// ============================================================================
// Optimizer Stats Test
// ============================================================================

TEST_F(OptimizerTest, ResetStatsClearsCounters) {
  auto program = parse("let x = 2 + 3;");

  Optimizer optimizer;
  optimizer.run(*program);

  EXPECT_GT(optimizer.getStats().constantsFolded, 0);

  optimizer.resetStats();
  EXPECT_EQ(optimizer.getStats().constantsFolded, 0);
  EXPECT_EQ(optimizer.getStats().deadCodeRemoved, 0);
  EXPECT_EQ(optimizer.getStats().functionsInlined, 0);
}
