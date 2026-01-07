#include "vm.h"
#include "profiler.h"
#include <sstream>
#include <stdexcept>

// ============================================================================
// Stack Operations
// ============================================================================

void VirtualMachine::push(Value value) {
  checkStackOverflow();
  stack_.push_back(std::move(value));
}

Value VirtualMachine::pop() {
  checkStackUnderflow();
  Value value = std::move(stack_.back());
  stack_.pop_back();
  return value;
}

Value VirtualMachine::peek() const {
  checkStackUnderflow();
  return stack_.back();
}

void VirtualMachine::printValue(const Value &value, std::ostream &os) const {
  if (value.isVoid()) {
    os << "void";
  } else if (value.isInt()) {
    os << value.asInt();
  } else if (value.isString()) {
    os << value.asString();
  } else if (value.isArray()) {
    os << "[";
    auto arr = value.asArray();
    for (size_t i = 0; i < arr->size(); ++i) {
      if (i > 0)
        os << ", ";
      printValue((*arr)[i], os);
    }
    os << "]";
  }
}

void VirtualMachine::checkStackOverflow() const {
  if (stack_.size() >= MAX_STACK_SIZE) {
    throw VMError("Stack overflow");
  }
}

void VirtualMachine::checkStackUnderflow() const {
  if (stack_.empty()) {
    throw VMError("Stack underflow");
  }
}

// ============================================================================
// Main Execution Loop
// ============================================================================

Value VirtualMachine::execute(const BytecodeProgram &program,
                              Profiler *profiler, bool keepState) {
  // Reset state
  stack_.clear();
  callStack_.clear();
  outputValues_.clear();

  if (!keepState) {
    locals_.clear();
    // Initialize locals with enough space
    locals_.resize(MAX_VARIABLES, Value(0));
  } else {
    // Ensure locals have enough space if MAX_VARIABLES increased (unlikely but
    // safe)
    if (locals_.size() < MAX_VARIABLES) {
      locals_.resize(MAX_VARIABLES, Value(0));
    }
  }

  // Start execution at main entry point
  uint16_t ip = program.mainEntry;
  uint16_t basePointer = 0;

  while (ip < program.code.size()) {
    const Instruction &instr = program.code[ip];
    Opcode op = static_cast<Opcode>(instr.opcode);
    uint16_t operand = instr.operand;

    // Profile if enabled
    if (profiler) {
      profiler->onExecute(op);
    }

    switch (op) {
    case Opcode::CONST: {
      // Push constant value onto stack
      if (operand >= program.constants.size()) {
        throw VMError("Invalid constant index");
      }
      push(program.constants[operand]);
      ++ip;
      break;
    }

    case Opcode::LOAD: {
      // Load local variable onto stack
      uint16_t slot = basePointer + operand;
      if (slot >= locals_.size()) {
        throw VMError("Invalid local variable index");
      }
      push(locals_[slot]);
      ++ip;
      break;
    }

    case Opcode::STORE: {
      // Pop value and store in local variable
      Value value = pop();
      uint16_t slot = basePointer + operand;
      if (slot >= locals_.size()) {
        throw VMError("Invalid local variable index");
      }
      locals_[slot] = std::move(value);
      ++ip;
      break;
    }

    case Opcode::ADD: {
      Value b = pop();
      Value a = pop();
      if (a.isInt() && b.isInt()) {
        push(Value(a.asInt() + b.asInt()));
      } else if (a.isString() && b.isString()) {
        push(Value(a.asString() + b.asString()));
      } else {
        throw VMError("Type mismatch for ADD");
      }
      ++ip;
      break;
    }

    case Opcode::SUB: {
      Value b = pop();
      Value a = pop();
      push(Value(a.asInt() - b.asInt()));
      ++ip;
      break;
    }

    case Opcode::MUL: {
      Value b = pop();
      Value a = pop();
      push(Value(a.asInt() * b.asInt()));
      ++ip;
      break;
    }

    case Opcode::DIV: {
      Value b = pop();
      Value a = pop();
      if (b.asInt() == 0) {
        throw VMError("Division by zero");
      }
      push(Value(a.asInt() / b.asInt()));
      ++ip;
      break;
    }

    case Opcode::MOD: {
      Value b = pop();
      Value a = pop();
      if (b.asInt() == 0) {
        throw VMError("Modulo by zero");
      }
      push(Value(a.asInt() % b.asInt()));
      ++ip;
      break;
    }

    case Opcode::JUMP: {
      // Unconditional jump
      ip = operand;
      break;
    }

    case Opcode::JUMP_IF_ZERO: {
      // Conditional jump if top of stack is zero
      Value value = pop();
      if (value.isInt() && value.asInt() == 0) {
        ip = operand;
      } else {
        ++ip;
      }
      break;
    }

    case Opcode::CALL: {
      // Function call
      if (operand >= program.functions.size()) {
        throw VMError("Invalid function index");
      }

      const FunctionInfo &fn = program.functions[operand];

      // Save current frame
      CallFrame frame;
      frame.ip = ip + 1; // Return to instruction after CALL
      frame.basePointer = basePointer;
      frame.funcIndex = operand;
      callStack_.push_back(frame);

      // Set up new frame
      uint16_t newBase =
          static_cast<uint16_t>(basePointer + fn.localCount + 16); // Some room

      // Pop arguments and place in new frame's locals
      for (int i = fn.arity - 1; i >= 0; --i) {
        Value arg = pop();
        locals_[newBase + i] = arg;
      }

      basePointer = newBase;
      ip = fn.entry;
      break;
    }

    case Opcode::RETURN: {
      // Function return
      Value returnValue = pop();

      if (callStack_.empty()) {
        // Returning from main - program ends
        return returnValue;
      }

      // Restore caller frame
      CallFrame frame = callStack_.back();
      callStack_.pop_back();

      ip = frame.ip;
      basePointer = frame.basePointer;

      // Push return value for caller
      push(returnValue);
      break;
    }

    case Opcode::PRINT: {
      // Print top of stack
      Value value = pop();
      printValue(value, *output_);
      *output_ << std::endl;
      outputValues_.push_back(value);
      ++ip;
      break;
    }

    case Opcode::EQ: {
      Value b = pop();
      Value a = pop();
      push(Value(a == b ? 1 : 0));
      ++ip;
      break;
    }

    case Opcode::NEQ: {
      Value b = pop();
      Value a = pop();
      push(Value(a != b ? 1 : 0));
      ++ip;
      break;
    }

    case Opcode::LT: {
      Value b = pop();
      Value a = pop();
      if (!a.isInt() || !b.isInt())
        throw VMError("Type error in comparison");
      push(Value(a.asInt() < b.asInt() ? 1 : 0));
      ++ip;
      break;
    }

    case Opcode::LTE: {
      Value b = pop();
      Value a = pop();
      if (!a.isInt() || !b.isInt())
        throw VMError("Type error in comparison");
      push(Value(a.asInt() <= b.asInt() ? 1 : 0));
      ++ip;
      break;
    }

    case Opcode::GT: {
      Value b = pop();
      Value a = pop();
      if (!a.isInt() || !b.isInt())
        throw VMError("Type error in comparison");
      push(Value(a.asInt() > b.asInt() ? 1 : 0));
      ++ip;
      break;
    }

    case Opcode::GTE: {
      Value b = pop();
      Value a = pop();
      if (!a.isInt() || !b.isInt())
        throw VMError("Type error in comparison");
      push(Value(a.asInt() >= b.asInt() ? 1 : 0));
      ++ip;
      break;
    }

    case Opcode::BUILD_ARRAY: {
      uint16_t count = operand;
      auto array = std::make_shared<std::vector<Value>>();
      array->resize(count);
      // Stack has elements in order ... e1, e2, e3 (top)
      // We pop e3, e2, e1. So fill from back.
      for (int i = count - 1; i >= 0; --i) {
        (*array)[i] = pop();
      }
      push(Value(array));
      ++ip;
      break;
    }

    case Opcode::ARRAY_LOAD: {
      Value index = pop();
      Value array = pop();
      if (!array.isArray()) {
        throw VMError("Runtime Error: Expected array for indexing");
      }
      if (!index.isInt()) {
        throw VMError("Runtime Error: Array index must be an integer");
      }
      auto &vec = *array.asArray();
      int idx = index.asInt();
      if (idx < 0 || static_cast<size_t>(idx) >= vec.size()) {
        throw VMError("Runtime Error: Array index out of bounds");
      }
      push(vec[idx]);
      ++ip;
      break;
    }

    case Opcode::ARRAY_STORE: {
      Value value = pop();
      Value index = pop();
      Value array = pop();
      if (!array.isArray()) {
        throw VMError("Runtime Error: Expected array for assignment");
      }
      if (!index.isInt()) {
        throw VMError("Runtime Error: Array index must be an integer");
      }
      auto &vec = *array.asArray();
      int idx = index.asInt();
      if (idx < 0 || static_cast<size_t>(idx) >= vec.size()) {
        throw VMError("Runtime Error: Array index out of bounds");
      }
      vec[idx] = std::move(value);
      ++ip;
      break;
    }

    case Opcode::POP: {
      pop();
      ++ip;
      break;
    }

    default:
      throw VMError("Unknown opcode: " + std::to_string(instr.opcode));
    }
  }

  // If we reach here, return top of stack or Void
  return stack_.empty() ? Value() : pop();
}
