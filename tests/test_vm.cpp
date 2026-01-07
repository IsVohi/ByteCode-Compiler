#include "codegen.h"
#include "vm.h"
#include <gtest/gtest.h>
#include <sstream>

class VMTest : public ::testing::Test {
protected:
  VirtualMachine vm;
  std::stringstream output;

  void SetUp() override { vm.setOutputStream(output); }

  BytecodeProgram makeProgram(std::vector<Instruction> code,
                              std::vector<Value> constants = {},
                              std::vector<FunctionInfo> functions = {}) {
    BytecodeProgram prog;
    prog.code = std::move(code);
    prog.constants = std::move(constants);
    prog.functions = std::move(functions);
    prog.mainEntry = 0;
    return prog;
  }

  Instruction instr(Opcode op, uint16_t operand = 0) {
    return {static_cast<uint8_t>(op), operand};
  }
};

// ============================================================================
// Arithmetic Tests
// ============================================================================

TEST_F(VMTest, AddTwoNumbers) {
  auto prog = makeProgram(
      {
          instr(Opcode::CONST, 0), // Push 3
          instr(Opcode::CONST, 1), // Push 5
          instr(Opcode::ADD),      // 3 + 5 = 8
          instr(Opcode::RETURN)    // Return 8
      },
      {3, 5});

  Value result = vm.execute(prog);
  EXPECT_TRUE(result.isInt());
  EXPECT_EQ(result.asInt(), 8);
}

TEST_F(VMTest, SubtractNumbers) {
  auto prog = makeProgram({instr(Opcode::CONST, 0), // Push 10
                           instr(Opcode::CONST, 1), // Push 4
                           instr(Opcode::SUB),      // 10 - 4 = 6
                           instr(Opcode::RETURN)},
                          {10, 4});

  EXPECT_EQ(vm.execute(prog).asInt(), 6);
}

TEST_F(VMTest, MultiplyNumbers) {
  auto prog = makeProgram({instr(Opcode::CONST, 0), instr(Opcode::CONST, 1),
                           instr(Opcode::MUL), instr(Opcode::RETURN)},
                          {7, 6});

  EXPECT_EQ(vm.execute(prog), 42);
}

TEST_F(VMTest, DivideNumbers) {
  auto prog = makeProgram({instr(Opcode::CONST, 0), instr(Opcode::CONST, 1),
                           instr(Opcode::DIV), instr(Opcode::RETURN)},
                          {20, 4});

  EXPECT_EQ(vm.execute(prog).asInt(), 5);
}

TEST_F(VMTest, ModuloNumbers) {
  auto prog = makeProgram({instr(Opcode::CONST, 0), instr(Opcode::CONST, 1),
                           instr(Opcode::MOD), instr(Opcode::RETURN)},
                          {17, 5});

  EXPECT_EQ(vm.execute(prog).asInt(), 2);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(VMTest, DivisionByZeroThrows) {
  auto prog = makeProgram({instr(Opcode::CONST, 0), instr(Opcode::CONST, 1),
                           instr(Opcode::DIV), instr(Opcode::RETURN)},
                          {10, 0});

  EXPECT_THROW(vm.execute(prog), VMError);
}

TEST_F(VMTest, ModuloByZeroThrows) {
  auto prog = makeProgram({instr(Opcode::CONST, 0), instr(Opcode::CONST, 1),
                           instr(Opcode::MOD), instr(Opcode::RETURN)},
                          {10, 0});

  EXPECT_THROW(vm.execute(prog), VMError);
}

// ============================================================================
// Print Tests
// ============================================================================

TEST_F(VMTest, PrintOutputsValue) {
  auto prog = makeProgram({instr(Opcode::CONST, 0), instr(Opcode::PRINT),
                           instr(Opcode::CONST, 1), instr(Opcode::RETURN)},
                          {42, 0});

  vm.execute(prog);

  auto outputs = vm.getOutput();
  ASSERT_EQ(outputs.size(), static_cast<size_t>(1));
  EXPECT_EQ(outputs[0].asInt(), 42);
}

// ============================================================================
// Variable Tests
// ============================================================================

TEST_F(VMTest, StoreAndLoadVariable) {
  auto prog = makeProgram({instr(Opcode::CONST, 0), // Push 100
                           instr(Opcode::STORE, 0), // Store to slot 0
                           instr(Opcode::LOAD, 0),  // Load from slot 0
                           instr(Opcode::RETURN)},
                          {100});

  EXPECT_EQ(vm.execute(prog), 100);
}

// ============================================================================
// Jump Tests
// ============================================================================

TEST_F(VMTest, UnconditionalJump) {
  auto prog = makeProgram({instr(Opcode::JUMP, 2),  // Jump to instruction 2
                           instr(Opcode::CONST, 0), // Skipped: Push 1
                           instr(Opcode::CONST, 1), // Push 42
                           instr(Opcode::RETURN)},
                          {1, 42});

  EXPECT_EQ(vm.execute(prog), 42);
}

TEST_F(VMTest, JumpIfZeroTrue) {
  auto prog = makeProgram({instr(Opcode::CONST, 0),        // Push 0
                           instr(Opcode::JUMP_IF_ZERO, 4), // Jump to 4 if zero
                           instr(Opcode::CONST, 1),        // Skipped
                           instr(Opcode::RETURN),          // Skipped
                           instr(Opcode::CONST, 2),        // Push 100
                           instr(Opcode::RETURN)},
                          {0, 50, 100});

  EXPECT_EQ(vm.execute(prog), 100);
}

TEST_F(VMTest, JumpIfZeroFalse) {
  auto prog =
      makeProgram({instr(Opcode::CONST, 0),        // Push 1 (non-zero)
                   instr(Opcode::JUMP_IF_ZERO, 4), // Don't jump
                   instr(Opcode::CONST, 1),        // Push 50
                   instr(Opcode::RETURN), instr(Opcode::CONST, 2), // Skipped
                   instr(Opcode::RETURN)},
                  {1, 50, 100});

  EXPECT_EQ(vm.execute(prog).asInt(), 50);
}

// ============================================================================
// Function Call Tests
// ============================================================================

TEST_F(VMTest, SimpleFunctionCall) {
  FunctionInfo fn{"double_it", 0, 1, 1};

  auto prog = makeProgram(
      {// Function at index 0: double_it(x) returns x + x
       instr(Opcode::LOAD, 0), // Load parameter
       instr(Opcode::LOAD, 0), // Load parameter again
       instr(Opcode::ADD), instr(Opcode::RETURN),

       // Main entry at index 4
       instr(Opcode::CONST, 0), // Push 21
       instr(Opcode::CALL, 0),  // Call double_it
       instr(Opcode::RETURN)},
      {21}, {fn});

  prog.mainEntry = 4;

  EXPECT_EQ(vm.execute(prog), 42);
}
