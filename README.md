# Optimizing Bytecode Compiler

A high-performance C++17 bytecode compiler with optimization passes for a stack-based virtual machine.

## Project Overview

This project implements a complete compilation pipeline from source to optimized bytecode execution. The compiler supports a custom high-level language with functions, arrays, loops, and more.

### Key Features

- **Stack-Based Bytecode VM**: 17 opcodes for arithmetic, control, arrays, functions, and I/O  
- **Modular Design**: Separate lexer, parser, codegen, optimizer, VM  
- **Full Language Support**: Functions, recursion, arrays, loops, conditionals  
- **Interactive REPL**: Persistent state across commands  
- **Robust Error Handling**: Custom exceptions per compilation stage  
- **Testing**: GoogleTest suite with 150+ tests  
- **Benchmark Suite**: Performance profiling and HTML dashboard  

## Example Syntax

```javascript
// Variables and arithmetic
let x = 10;
let y = 20;
print(x + y);           // 30

// Functions and recursion
fn factorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}
print(factorial(5));    // 120

// Arrays
let arr = [1, 2, 3, 4, 5];
arr[0] = 99;
print(arr);             // [99, 2, 3, 4, 5]

// Nested arrays
let matrix = [[1, 2], [3, 4]];
print(matrix[1][0]);    // 3

// Control flow
for (let i = 0; i < 5; i = i + 1) {
    if (i == 2) { continue; }
    print(i);
}

// While loops with break
while (x > 0) {
    if (x == 5) { break; }
    x = x - 1;
}
```

## Build Instructions

### Prerequisites

- CMake 3.16+  
- C++17 compiler (GCC 7+, Clang 5+, MSVC 2017+)  
- Git  

### Building

```bash
mkdir build && cd build
cmake ..
make
```

### Running Compiler

```bash
# Run a source file
./build/compiler script.src

# Run with optimizations disabled
./build/compiler script.src --no-opt

# Run with profiling enabled
./build/compiler script.src --profile

# Run with verbose output
./build/compiler script.src --verbose
```

### Open in New Terminal Window (macOS)

```bash
# Open REPL in a new Terminal window
./run.sh

# Run a file in a new Terminal window
./run.sh examples/comprehensive_demo.txt
```

### Interactive REPL

```bash
./compiler
# ByteCode Compiler REPL v1.0.0
# > let x = 5;
# > print(x * 2);
# 10
# > exit
```

### Running Tests

```bash
cd build
./tests
ctest --output-on-failure
```

## Architecture

```
Source → Lexer → Parser → AST → Optimizer → Codegen → Bytecode → VM
```

Core components status:

| Component | Status          |
|-----------|-----------------|
| Lexer     | ✅ Implemented   |
| Parser    | ✅ Implemented   |
| Codegen   | ✅ Implemented   |
| Optimizer | ✅ Implemented   |
| VM        | ✅ Implemented   |
| Arrays    | ✅ Implemented   |
| REPL      | ✅ Implemented   |
| CLI       | ✅ Implemented   |
| Tests     | ✅ 150+ tests    |

## Project Structure

```
ByteCode-Compiler/
├── src/
│   ├── main.cpp        # CLI and REPL
│   ├── lexer.cpp       # Tokenizer
│   ├── parser.cpp      # Recursive descent parser
│   ├── ast.cpp         # AST node implementations
│   ├── codegen.cpp     # Bytecode generator
│   ├── optimizer.cpp   # Optimization passes
│   └── vm.cpp          # Virtual machine
├── include/
│   ├── common.h        # Types, opcodes, exceptions
│   ├── lexer.h
│   ├── parser.h
│   ├── ast.h
│   ├── codegen.h
│   ├── optimizer.h
│   ├── vm.h
│   └── profiler.h
├── tests/
│   ├── test_lexer.cpp
│   ├── test_parser.cpp
│   ├── test_codegen.cpp
│   ├── test_vm.cpp
│   ├── test_optimizer.cpp
│   ├── test_arrays.cpp
│   ├── test_control_flow.cpp
│   ├── test_bubblesort.cpp
│   └── test_end_to_end.cpp
├── examples/
│   └── comprehensive_demo.txt
├── benchmarks/
│   └── benchmark.py
├── results/
│   └── dashboard.html
├── CMakeLists.txt
└── README.md
```

## Bytecode Specification

| Opcode        | Hex  | Description                  |
|---------------|------|------------------------------|
| CONST         | 0x00 | Push constant                |
| LOAD          | 0x01 | Load variable                |
| STORE         | 0x02 | Store variable               |
| ADD           | 0x03 | Pop two and push sum         |
| SUB           | 0x04 | Pop two and push difference  |
| MUL           | 0x05 | Pop two and push product     |
| DIV           | 0x06 | Pop two and push quotient    |
| MOD           | 0x07 | Pop two and push modulo      |
| JUMP          | 0x08 | Unconditional jump           |
| JUMP_IF_ZERO  | 0x09 | Jump if top is zero          |
| CALL          | 0x0A | Call function                |
| RETURN        | 0x0B | Return from function         |
| PRINT         | 0x0C | Pop and print top            |
| EQ/NEQ/LT/... | 0x0D-0x12 | Comparison operators    |
| BUILD_ARRAY   | 0x13 | Build array from stack       |
| ARRAY_LOAD    | 0x14 | Load from array index        |
| ARRAY_STORE   | 0x15 | Store to array index         |
| POP           | 0x16 | Pop and discard top          |

### Limits

- Stack Size: 256  
- Variables: 1024 per scope  
- Instructions: 65535 per program  
- Functions: 256 per program  
- Bytecode Version: 1  

## Compiler Flags

| Flag        | Description                    |
|-------------|--------------------------------|
| `--no-opt`  | Disable optimizations          |
| `--profile` | Enable execution profiling     |
| `--verbose` | Print detailed compilation info|
| `--dump`    | Dump generated bytecode        |

## Optimizations

The optimizer performs several passes:

- **Constant Folding**: Evaluates constant expressions at compile time
- **Dead Code Elimination**: Removes unreachable code after return statements
- **Unused Variable Detection**: Identifies and warns about unused variables
- **Function Inlining**: Inlines small, non-recursive functions

## Error Handling

Custom exception classes for each stage:

| Exception       | Stage           |
|-----------------|-----------------|
| `CompilerError` | Base class      |
| `LexerError`    | Lexical errors  |
| `ParserError`   | Syntax errors   |
| `CodegenError`  | Code generation |
| `OptimizerError`| Optimization    |
| `VMError`       | Runtime errors  |

Example errors:
```
> print(x / 0);
Error: VM error: Division by zero

> let arr = [1, 2]; print(arr[10]);
Error: VM error: Array index out of bounds
```

## Benchmarks

Run benchmarks and view results:

```bash
cd benchmarks
python benchmark.py
# Results written to ../results/benchmarks.csv
# Open ../results/dashboard.html for charts
```

## Contributing Guidelines

- Maintain C++17 compliance  
- Add tests for new features  
- Keep warnings as errors (`-Werror`)  
- Document public APIs  
- Follow existing code style  

---

**Status**: Fully implemented and tested. All 150+ tests passing.
