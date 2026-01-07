#ifndef COMPILER_COMMON_H
#define COMPILER_COMMON_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

#include <variant>

#include <memory>
#include <vector>

// ============================================================================
// Value System
// ============================================================================

struct Value;
using ArrayPtr = std::shared_ptr<std::vector<Value>>;

/**
 * Represents a runtime value (integer, string, or array)
 */
struct Value {
  std::variant<std::monostate, int32_t, std::string, ArrayPtr> data;

  // Constructors
  Value() : data(std::monostate{}) {}
  Value(int32_t v) : data(v) {}
  Value(std::string v) : data(std::move(v)) {}
  Value(const char *v) : data(std::string(v)) {}
  Value(ArrayPtr v) : data(std::move(v)) {}

  // Type checks
  bool isVoid() const { return std::holds_alternative<std::monostate>(data); }
  bool isInt() const { return std::holds_alternative<int32_t>(data); }
  bool isString() const { return std::holds_alternative<std::string>(data); }
  bool isArray() const { return std::holds_alternative<ArrayPtr>(data); }

  // Accessors
  int32_t asInt() const {
    if (!isInt())
      throw std::runtime_error("Type error: expected int");
    return std::get<int32_t>(data);
  }

  const std::string &asString() const {
    if (!isString())
      throw std::runtime_error("Type error: expected string");
    return std::get<std::string>(data);
  }

  ArrayPtr asArray() const {
    if (!isArray())
      throw std::runtime_error("Type error: expected array");
    return std::get<ArrayPtr>(data);
  }

  // Equality
  bool operator==(const Value &other) const {
    if (index() != other.index())
      return false;
    if (isVoid())
      return true;
    if (isInt())
      return asInt() == other.asInt();
    if (isString())
      return asString() == other.asString();
    if (isArray())
      return asArray() == other.asArray(); // Pointer equality
    return false;
  }

  bool operator!=(const Value &other) const { return !(*this == other); }

  size_t index() const { return data.index(); }
};

// ============================================================================
// Bytecode Definitions
// ============================================================================

/**
 * Bytecode opcodes for the virtual machine
 */
enum class Opcode : uint8_t {
  CONST = 0x00,        // Load constant value
  LOAD = 0x01,         // Load variable
  STORE = 0x02,        // Store to variable
  ADD = 0x03,          // Addition
  SUB = 0x04,          // Subtraction
  MUL = 0x05,          // Multiplication
  DIV = 0x06,          // Division
  MOD = 0x07,          // Modulo
  JUMP = 0x08,         // Unconditional jump
  JUMP_IF_ZERO = 0x09, // Conditional jump if stack top is zero
  CALL = 0x0A,         // Function call
  RETURN = 0x0B,       // Function return
  PRINT = 0x0C,        // Print stack top
  EQ = 0x0D,           // Equal
  NEQ = 0x0E,          // Not Equal
  LT = 0x0F,           // Less Than
  LTE = 0x10,          // Less Than or Equal
  GT = 0x11,           // Greater Than
  GTE = 0x12,          // Greater Than or Equal
  BUILD_ARRAY = 0x13,  // Build Array
  ARRAY_LOAD = 0x14,   // Load from Array
  ARRAY_STORE = 0x15,  // Store to Array
  POP = 0x16           // Pop stack
};

/**
 * Bytecode instruction structure
 */
struct Instruction {
  uint8_t opcode; // Operation code
  uint16_t
      operand; // Operand (immediate value, variable index, jump offset, etc.)
};

// ============================================================================
// System Limits and Constants
// ============================================================================

// Virtual machine constraints
constexpr uint16_t MAX_STACK_SIZE = 256;
constexpr uint16_t MAX_VARIABLES = 1024;
constexpr uint16_t MAX_INSTRUCTIONS = 65535;
constexpr uint16_t MAX_FUNCTIONS = 256;

// Bytecode version for compatibility checks
constexpr uint8_t BYTECODE_VERSION = 1;

// ============================================================================
// Exception Classes
// ============================================================================

/**
 * Base exception for compiler errors
 */
class CompilerError : public std::runtime_error {
public:
  explicit CompilerError(const std::string &message)
      : std::runtime_error(message) {}
};

/**
 * Exception raised during lexical analysis
 */
class LexerError : public CompilerError {
public:
  explicit LexerError(const std::string &message)
      : CompilerError("Lexer error: " + message) {}
};

/**
 * Exception raised during parsing
 */
class ParserError : public CompilerError {
public:
  explicit ParserError(const std::string &message)
      : CompilerError("Parser error: " + message) {}
};

/**
 * Exception raised during code generation
 */
class CodegenError : public CompilerError {
public:
  explicit CodegenError(const std::string &message)
      : CompilerError("Codegen error: " + message) {}
};

/**
 * Exception raised during optimization
 */
class OptimizerError : public CompilerError {
public:
  explicit OptimizerError(const std::string &message)
      : CompilerError("Optimizer error: " + message) {}
};

/**
 * Exception raised during bytecode execution
 */
class VMError : public CompilerError {
public:
  explicit VMError(const std::string &message)
      : CompilerError("VM error: " + message) {}
};

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Convert Opcode enum to human-readable string
 * @param opcode The opcode to convert
 * @return String representation of the opcode
 */
constexpr std::string_view opcode_to_string(Opcode opcode) noexcept {
  switch (opcode) {
  case Opcode::CONST:
    return "CONST";
  case Opcode::LOAD:
    return "LOAD";
  case Opcode::STORE:
    return "STORE";
  case Opcode::ADD:
    return "ADD";
  case Opcode::SUB:
    return "SUB";
  case Opcode::MUL:
    return "MUL";
  case Opcode::DIV:
    return "DIV";
  case Opcode::MOD:
    return "MOD";
  case Opcode::JUMP:
    return "JUMP";
  case Opcode::JUMP_IF_ZERO:
    return "JUMP_IF_ZERO";
  case Opcode::CALL:
    return "CALL";
  case Opcode::RETURN:
    return "RETURN";
  case Opcode::PRINT:
    return "PRINT";
  case Opcode::EQ:
    return "EQ";
  case Opcode::NEQ:
    return "NEQ";
  case Opcode::LT:
    return "LT";
  case Opcode::LTE:
    return "LTE";
  case Opcode::GT:
    return "GT";
  case Opcode::GTE:
    return "GTE";
  case Opcode::BUILD_ARRAY:
    return "BUILD_ARRAY";
  case Opcode::ARRAY_LOAD:
    return "ARRAY_LOAD";
  case Opcode::ARRAY_STORE:
    return "ARRAY_STORE";
  default:
    return "UNKNOWN";
  }
}

#endif // COMPILER_COMMON_H
