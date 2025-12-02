# Optimizing Bytecode Compiler

A high-performance C++17 bytecode compiler with integrated optimization passes for an experimental stack-based virtual machine language.

## Project Overview

This project implements a complete compilation pipeline from source code to optimized bytecode suitable for execution on a stack-based virtual machine. The compiler is designed with modularity in mind, featuring distinct stages for lexical analysis, parsing, code generation, and optimization.

### Key Features

- **Stack-Based Bytecode VM**: 13 opcodes supporting arithmetic, control flow, function calls, and I/O
- **Modular Architecture**: Separate lexer, parser, code generator, and optimizer stages
- **Comprehensive Error Handling**: Custom exception hierarchy for all compilation stages
- **Performance Analysis**: Built-in profiling and timing infrastructure
- **Test Suite**: GoogleTest integration with empty test framework
- **Benchmark Suite**: Performance benchmarking utilities (framework not yet implemented)

## Language Example Syntax

The target language supports basic computational programming with variables, arithmetic operations, and function calls:

```
fn main() {
    x = 10
    y = 20
    z = x + y
    print z
}

fn factorial(n) {
    if n <= 1 {
        return 1
    }
    return n * factorial(n - 1)
}
```

**Note:** This is a placeholder syntax. Actual parser implementation will validate the true language specification.

## Build Instructions

### Prerequisites

- CMake 3.16 or later
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Git

### Building

```bash
# Clone or navigate to project directory
cd compiler

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build all targets
make

# Or on Windows with MSVC
cmake .. -G "Visual Studio 16 2019"
cmake --build .
```

### Running the Compiler

```bash
# Basic usage
./compiler input.src

# With optimization disabled
./compiler input.src --no-opt

# With profiling enabled
./compiler input.src --profile

# With verbose output
./compiler input.src --verbose

# Multiple flags
./compiler input.src --profile --verbose
```

### Running Tests

```bash
# Run all tests
make test_all

# Or using CMake test runner
ctest --verbose
```

## Architecture

### Compilation Pipeline

```
┌─────────────┐      ┌──────────┐      ┌────────┐      ┌───────────┐      ┌─────────┐
│   Source    │─────▶│  Lexer   │─────▶│ Parser │─────▶│ Codegen   │─────▶│Optimizer│─────▶ Bytecode
│    Code     │      │          │      │        │      │           │      │         │
└─────────────┘      └──────────┘      └────────┘      └───────────┘      └─────────┘
                          ▲                  ▲              ▲                   ▲
                          │                  │              │                   │
                          └──────────────────┴──────────────┴───────────────────┘
                                Error Reporting & Recovery
```

### Core Components

| Component | Purpose | Status |
|-----------|---------|--------|
| **Lexer** | Tokenization of source code | Not implemented |
| **Parser** | Syntax analysis and AST construction | Not implemented |
| **Codegen** | Translation to bytecode instructions | Not implemented |
| **Optimizer** | Bytecode optimization passes | Not implemented |
| **Common** | Shared definitions (opcodes, exceptions) | Implemented ✓ |
| **CLI** | Command-line interface | Implemented ✓ |
| **Tests** | Google Test framework setup | Implemented ✓ |

## Project Structure

```
compiler/
├── src/
│   └── main.cpp                 # CLI entry point
├── include/
│   └── common.h                 # Shared definitions & bytecode specs
├── tests/
│   └── test_runner.cpp          # Test suite (placeholder)
├── benchmarks/
│   └── (empty - placeholder)    # Performance benchmarks
├── results/
│   └── (empty - for output)     # Analysis results and reports
├── CMakeLists.txt               # Build configuration
├── .gitignore                   # Git ignore rules
└── README.md                    # This file
```

## Bytecode Specification

### Opcodes

| Opcode | Hex | Operand | Description |
|--------|-----|---------|-------------|
| CONST | 0x00 | value | Push constant onto stack |
| LOAD | 0x01 | var_idx | Load variable onto stack |
| STORE | 0x02 | var_idx | Pop stack and store in variable |
| ADD | 0x03 | - | Pop 2 values, push sum |
| SUB | 0x04 | - | Pop 2 values, push difference |
| MUL | 0x05 | - | Pop 2 values, push product |
| DIV | 0x06 | - | Pop 2 values, push quotient |
| MOD | 0x07 | - | Pop 2 values, push modulo |
| JUMP | 0x08 | offset | Unconditional jump |
| JUMP_IF_ZERO | 0x09 | offset | Jump if stack top is zero |
| CALL | 0x0A | func_idx | Call function |
| RETURN | 0x0B | - | Return from function |
| PRINT | 0x0C | - | Pop and print stack top |

### Limits

- **Stack Size**: 256 entries
- **Variables**: 1024 per scope
- **Instructions**: 65535 per program
- **Functions**: 256 per program
- **Bytecode Version**: 1

## Compiler Configuration

The compiler supports the following command-line flags:

- `--no-opt`: Disable all optimization passes
- `--profile`: Enable performance profiling
- `--verbose`: Print detailed compilation information

## Next Steps (Upcoming Commits)

1. **Lexer Implementation**: Tokenization of source code
2. **Parser Implementation**: AST construction from token stream
3. **Code Generator**: Bytecode emission from AST
4. **Optimizer**: Constant folding, dead code elimination, etc.
5. **Virtual Machine**: Bytecode execution engine
6. **Test Suite**: Comprehensive unit and integration tests
7. **Benchmarks**: Performance measurement suite

## Error Handling

The compiler defines custom exception hierarchy for precise error reporting:

- `CompilerError`: Base exception class
- `LexerError`: Lexical analysis errors
- `ParserError`: Syntax errors
- `CodegenError`: Code generation errors
- `OptimizerError`: Optimization-related errors
- `VMError`: Runtime execution errors

## Contributing

This is a scaffolding commit with no compiler logic implemented. Future contributions should:

1. Implement each pipeline stage in separate commits
2. Add corresponding test cases
3. Maintain C++17 standards
4. Keep warnings-as-errors enabled
5. Document all public APIs

## License

[To be determined]

## Author

[Project maintainer information]

---

**Commit**: Initial Project Scaffold  
**Date**: [Auto-generated]  
**Status**: Ready for lexer implementation
