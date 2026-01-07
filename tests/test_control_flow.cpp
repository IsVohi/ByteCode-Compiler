#include "codegen.h"
#include "common.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include <gtest/gtest.h>

class ControlFlowTest : public ::testing::Test {
protected:
  // Helper to run code and get result
  Value run(const std::string &source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto program = parser.parseProgram();

    // No optimization for now to test raw codegen
    // Optimizer optimizer;
    // optimizer.run(*program);

    CodeGenerator codegen;
    auto bytecode = codegen.generate(*program);

    VirtualMachine vm;
    return vm.execute(bytecode);
  }

  std::vector<Value> getOutput(const std::string &source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto program = parser.parseProgram();

    CodeGenerator codegen;
    auto bytecode = codegen.generate(*program);

    VirtualMachine vm;
    vm.execute(bytecode);
    return vm.getOutput();
  }
};

TEST_F(ControlFlowTest, BasicForLoop) {
  // for (let i = 0; i < 5; i = i + 1) { print(i); }
  // Expect: 0, 1, 2, 3, 4
  std::string source = R"(
      for (let i = 0; i < 5; i = i + 1) {
        print(i);
      }
  )";

  auto output = getOutput(source);
  ASSERT_EQ(output.size(), 5);
  for (int i = 0; i < 5; ++i) {
    EXPECT_EQ(output[i].asInt(), i);
  }
}

TEST_F(ControlFlowTest, ForLoopWithBreak) {
  // for (let i = 0; i < 10; i = i + 1) { if (i == 3) { break; } print(i); }
  // Expect: 0, 1, 2
  std::string source = R"(
      for (let i = 0; i < 10; i = i + 1) {
        if (i == 3) { break; }
        print(i);
      }
  )";

  auto output = getOutput(source);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0].asInt(), 0);
  EXPECT_EQ(output[1].asInt(), 1);
  EXPECT_EQ(output[2].asInt(), 2);
}

TEST_F(ControlFlowTest, ForLoopWithContinue) {
  // for (let i = 0; i < 5; i = i + 1) { if (i == 2) { continue; } print(i); }
  // Expect: 0, 1, 3, 4
  std::string source = R"(
      for (let i = 0; i < 5; i = i + 1) {
        if (i == 2) { continue; }
        print(i);
      }
  )";

  auto output = getOutput(source);
  ASSERT_EQ(output.size(), 4);
  EXPECT_EQ(output[0].asInt(), 0);
  EXPECT_EQ(output[1].asInt(), 1);
  EXPECT_EQ(output[2].asInt(), 3);
  EXPECT_EQ(output[3].asInt(), 4);
}

TEST_F(ControlFlowTest, WhileLoopWithBreak) {
  std::string source = R"(
      let i = 0;
      while (i < 10) {
        if (i == 3) { break; }
        print(i);
        i = i + 1;
      }
  )";

  auto output = getOutput(source);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0].asInt(), 0);
  EXPECT_EQ(output[1].asInt(), 1);
  EXPECT_EQ(output[2].asInt(), 2);
}

TEST_F(ControlFlowTest, WhileLoopWithContinue) {
  // Be careful with continue in while loop: increment must happen before
  // continue or inside loop Here we put increment at start to avoid infinite
  // loop easily
  std::string source = R"(
      let i = -1;
      while (i < 4) {
        i = i + 1;
        if (i == 2) { continue; }
        print(i);
      }
  )";
  // Start: i=-1. Body: i=0, print 0.
  // Next: i=0<4. Body: i=1, print 1.
  // Next: i=1<4. Body: i=2, continue.
  // Next: i=2<4. Body: i=3, print 3.
  // Next: i=3<4. Body: i=4, print 4.
  // Next: i=4<4 -> False.
  // Output: 0, 1, 3, 4

  auto output = getOutput(source);
  ASSERT_EQ(output.size(), 4);
  EXPECT_EQ(output[0].asInt(), 0);
  EXPECT_EQ(output[1].asInt(), 1);
  EXPECT_EQ(output[2].asInt(), 3);
  EXPECT_EQ(output[3].asInt(), 4);
}

TEST_F(ControlFlowTest, NestedLoops) {
  // for (let i=0; i<3; i=i+1) { for (let j=0; j<3; j=j+1) { if (j==1) break;
  // print(i*10+j); } } i=0: j=0 -> print 0. j=1 -> break. i=1: j=0 -> print 10.
  // j=1 -> break. i=2: j=0 -> print 20. j=1 -> break. Output: 0, 10, 20
  std::string source = R"(
      for (let i = 0; i < 3; i = i + 1) {
        for (let j = 0; j < 3; j = j + 1) {
          if (j == 1) { break; }
          print(i * 10 + j);
        }
      }
  )";

  auto output = getOutput(source);
  ASSERT_EQ(output.size(), 3);
  EXPECT_EQ(output[0].asInt(), 0);
  EXPECT_EQ(output[1].asInt(), 10);
  EXPECT_EQ(output[2].asInt(), 20);
}
