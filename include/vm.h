#ifndef COMPILER_VM_H
#define COMPILER_VM_H

#include "codegen.h"
#include "common.h"
#include <cstdint>
#include <iostream>
#include <vector>

// Forward declaration
class Profiler;

/**
 * Call frame for function invocation
 */
struct CallFrame {
  uint16_t ip;          // Instruction pointer (return address)
  uint16_t basePointer; // Base of local variables in stack
  uint16_t funcIndex;   // Index into functions array
};

/**
 * Stack-based Virtual Machine for executing bytecode.
 * Implements all 13 opcodes defined in common.h.
 */
class VirtualMachine {
public:
  VirtualMachine() = default;

  /**
   * Execute a bytecode program
   * @param program The compiled bytecode program
   * @param profiler Optional profiler for instruction counting (nullptr to
   * disable)
   * @return The final value on the stack (program result)
   */
  Value execute(const BytecodeProgram &program, Profiler *profiler = nullptr,
                bool keepState = false);

  /**
   * Set output stream for PRINT (defaults to std::cout)
   */
  void setOutputStream(std::ostream &os) { output_ = &os; }

  /**
   * Get the last output printed (for testing)
   */
  const std::vector<Value> &getOutput() const { return outputValues_; }

private:
  std::vector<Value> stack_;          // Value stack
  std::vector<Value> locals_;         // Local variable storage
  std::vector<CallFrame> callStack_;  // Call frames for function calls
  std::ostream *output_ = &std::cout; // Output stream
  std::vector<Value> outputValues_;   // Captured output values

  // Stack operations
  void push(Value value);
  Value pop();
  Value peek() const;

  // Helper
  void printValue(const Value &value, std::ostream &os) const;

  // Validation
  void checkStackOverflow() const;
  void checkStackUnderflow() const;
};

#endif // COMPILER_VM_H
