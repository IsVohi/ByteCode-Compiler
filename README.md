# Optimizing Bytecode Compiler

A high-performance C++17 bytecode compiler with optimization passes for a stack-based virtual machine.

## Project Overview

This project implements a modular compilation pipeline from source to optimized bytecode. After the second commit, the **lexer** (tokenizer) is fully implemented and tested, capable of converting source code into tokens.

### Key Features

- **Stack-Based Bytecode VM**: 13 opcodes for arithmetic, control, functions, and I/O  
- **Modular Design**: Separate lexer, parser, codegen, optimizer  
- **Lexer Implemented**: Tokenizes keywords, identifiers, numbers, operators, delimiters  
- **Robust Error Handling**: Custom exceptions per compilation stage  
- **Testing**: GoogleTest suite with 50+ lexer tests  
- **Benchmark Suite**: Planned for future commits  

## Example Syntax

```cpp
fn main() {
    let x = 10;
    let y = 20;
    let z = x + y;
    print(z);
}

fn factorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
```

Note: Lexer recognizes tokens in this syntax; full parsing comes later.

## Build Instructions

### Prerequisites

- CMake 3.16+  
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)  
- Git  

### Building

```bash
cd compiler
mkdir build && cd build
cmake ..
make
```

### Running Compiler

```bash
./compiler input.src
./compiler input.src --no-opt
./compiler input.src --profile
./compiler input.src --verbose
```

Currently, the compiler parses options and prints config info, but does not compile code fully yet.

### Running Tests

```bash
make test_all
ctest --verbose
./tests
```

Tests focus on lexer functionality.

## Architecture

```
Source → Lexer (done)
       → Parser (TBD)
       → Codegen (TBD)
       → Optimizer (TBD)
       → Bytecode
```

Core components status:

| Component | Status          |
|-----------|-----------------|
| Lexer     | Implemented ✓   |
| Parser    | Not implemented |
| Codegen   | Not implemented |
| Optimizer | Not implemented |
| CLI       | Implemented ✓   |
| Tests     | Lexer tests ✓   |

## Project Structure

```
compiler/
├ src/
│  ├ main.cpp
│  └ lexer.cpp
├ include/
│  ├ common.h
│  └ lexer.h
├ tests/
│  ├ test_runner.cpp
│  └ test_lexer.cpp
├ benchmarks/ (empty)
├ results/ (empty)
├ CMakeLists.txt
├ .gitignore
└ README.md
```

## Bytecode Specification

| Opcode       | Hex  | Description                |
|--------------|------|----------------------------|
| CONST        | 0x00 | Push constant               |
| LOAD         | 0x01 | Load variable               |
| STORE        | 0x02 | Store variable              |
| ADD          | 0x03 | Pop two and push sum        |
| SUB          | 0x04 | Pop two and push difference |
| MUL          | 0x05 | Pop two and push product    |
| DIV          | 0x06 | Pop two and push quotient   |
| MOD          | 0x07 | Pop two and push modulo     |
| JUMP         | 0x08 | Unconditional jump          |
| JUMP_IF_ZERO | 0x09 | Jump if top is zero         |
| CALL         | 0x0A | Call function               |
| RETURN       | 0x0B | Return from function        |
| PRINT        | 0x0C | Pop and print top           |

Limits:

- Stack Size: 256  
- Variables: 1024 per scope  
- Instructions: 65535 per program  
- Functions: 256 per program  
- Bytecode Version: 1  

## Compiler Flags

- `--no-opt`: Disable optimizations  
- `--profile`: Enable profiling  
- `--verbose`: Print detailed info (lexer output integration future)  

## Implementation Status After Commit 2

- CLI inputs accepted, config shown  
- Lexer recognizes tokens (keywords, identifiers, numbers, operators, delimiters)  
- Skips whitespace and comments  
- Tracks line/column for tokens  
- Reports errors for illegal characters  
- 50+ GoogleTest lexer tests pass  
- Parser, codegen, optimizer, VM still to implement  

## Next Steps

1. Implement Parser (token stream → AST)  
2. Code Generator (AST → bytecode)  
3. Optimizer (code improvements)  
4. Virtual Machine (bytecode execution)  
5. Expand testing suite for all stages  
6. Add benchmark tools  

## Error Handling

Custom exception classes:

- `CompilerError` (base)  
- `LexerError` (lexical errors)  
- `ParserError` (syntax errors)  
- `CodegenError` (code generation)  
- `OptimizerError` (optimization)  
- `VMError` (runtime)  

## Contributing Guidelines

- Implement pipeline stages incrementally  
- Add tests per stage  
- Maintain C++17 compliance  
- Keep warnings as errors  
- Document public APIs and design  

---

**Commit Status**: Lexer implemented and tested, ready for parser development.
