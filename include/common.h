#ifndef COMPILER_COMMON_H
#define COMPILER_COMMON_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

// ============================================================================
// Bytecode Definitions
// ============================================================================

/**
 * Bytecode opcodes for the virtual machine
 */
enum class Opcode : uint8_t {
    CONST       = 0x00,  // Load constant value
    LOAD        = 0x01,  // Load variable
    STORE       = 0x02,  // Store to variable
    ADD         = 0x03,  // Addition
    SUB         = 0x04,  // Subtraction
    MUL         = 0x05,  // Multiplication
    DIV         = 0x06,  // Division
    MOD         = 0x07,  // Modulo
    JUMP        = 0x08,  // Unconditional jump
    JUMP_IF_ZERO= 0x09,  // Conditional jump if stack top is zero
    CALL        = 0x0A,  // Function call
    RETURN      = 0x0B,  // Function return
    PRINT       = 0x0C   // Print stack top
};

/**
 * Bytecode instruction structure
 */
struct Instruction {
    uint8_t opcode;   // Operation code
    uint16_t operand; // Operand (immediate value, variable index, jump offset, etc.)
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
    explicit CompilerError(const std::string& message)
        : std::runtime_error(message) {}
    explicit CompilerError(std::string_view message)
        : std::runtime_error(std::string(message)) {}
};

/**
 * Exception raised during lexical analysis
 */
class LexerError : public CompilerError {
public:
    explicit LexerError(const std::string& message)
        : CompilerError("Lexer error: " + message) {}
    explicit LexerError(std::string_view message)
        : CompilerError(std::string("Lexer error: ") + std::string(message)) {}
};

/**
 * Exception raised during parsing
 */
class ParserError : public CompilerError {
public:
    explicit ParserError(const std::string& message)
        : CompilerError("Parser error: " + message) {}
    explicit ParserError(std::string_view message)
        : CompilerError(std::string("Parser error: ") + std::string(message)) {}
};

/**
 * Exception raised during code generation
 */
class CodegenError : public CompilerError {
public:
    explicit CodegenError(const std::string& message)
        : CompilerError("Codegen error: " + message) {}
    explicit CodegenError(std::string_view message)
        : CompilerError(std::string("Codegen error: ") + std::string(message)) {}
};

/**
 * Exception raised during optimization
 */
class OptimizerError : public CompilerError {
public:
    explicit OptimizerError(const std::string& message)
        : CompilerError("Optimizer error: " + message) {}
    explicit OptimizerError(std::string_view message)
        : CompilerError(std::string("Optimizer error: ") + std::string(message)) {}
};

/**
 * Exception raised during bytecode execution
 */
class VMError : public CompilerError {
public:
    explicit VMError(const std::string& message)
        : CompilerError("VM error: " + message) {}
    explicit VMError(std::string_view message)
        : CompilerError(std::string("VM error: ") + std::string(message)) {}
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
        case Opcode::CONST:         return "CONST";
        case Opcode::LOAD:          return "LOAD";
        case Opcode::STORE:         return "STORE";
        case Opcode::ADD:           return "ADD";
        case Opcode::SUB:           return "SUB";
        case Opcode::MUL:           return "MUL";
        case Opcode::DIV:           return "DIV";
        case Opcode::MOD:           return "MOD";
        case Opcode::JUMP:          return "JUMP";
        case Opcode::JUMP_IF_ZERO:  return "JUMP_IF_ZERO";
        case Opcode::CALL:          return "CALL";
        case Opcode::RETURN:        return "RETURN";
        case Opcode::PRINT:         return "PRINT";
        default:                    return "UNKNOWN";
    }
}

#endif // COMPILER_COMMON_H
