# Architecture Guide

This document describes the internal architecture of the Optimizing Bytecode Compiler.

## Pipeline Overview

```
Source Code → Lexer → Parser → AST → Optimizer → CodeGen → Bytecode → VM → Result
                                                                      ↓
                                                                    REPL
```

## Components

### 1. Lexer (`lexer.h`, `lexer.cpp`)
Converts source text into tokens. Handles:
- **Keywords**: `fn`, `let`, `if`, `else`, `while`, `for`, `return`, `print`, `break`, `continue`
- **Operators**: `+`, `-`, `*`, `/`, `%`, `<`, `>`, `<=`, `>=`, `==`, `!=`, `&&`, `||`, `!`
- **Literals**: integers, strings
- **Identifiers**: variable and function names
- **Delimiters**: `(`, `)`, `{`, `}`, `[`, `]`, `;`, `,`

### 2. Parser (`parser.h`, `parser.cpp`)
Recursive-descent parser that builds an Abstract Syntax Tree.

**Expression Precedence** (lowest to highest):
1. Logical OR (`||`)
2. Logical AND (`&&`)
3. Equality (`==`, `!=`)
4. Relational (`<`, `<=`, `>`, `>=`)
5. Additive (`+`, `-`)
6. Multiplicative (`*`, `/`, `%`)
7. Unary (`-`, `!`)
8. Postfix (array indexing `[]`, function calls `()`)
9. Primary (literals, identifiers, arrays, parentheses)

### 3. AST (`ast.h`, `ast.cpp`)
Abstract Syntax Tree node hierarchy:
- **Expressions**: `NumberExpr`, `StringLiteralExpr`, `IdentifierExpr`, `BinaryOpExpr`, `UnaryOpExpr`, `FunctionCallExpr`, `ArrayLiteralExpr`, `IndexExpr`
- **Statements**: `AssignmentStmt`, `ArrayAssignmentStmt`, `PrintStmt`, `IfStmt`, `WhileStmt`, `ForStmt`, `ReturnStmt`, `BreakStmt`, `ContinueStmt`, `ExpressionStmt`, `BlockStmt`
- **Top-level**: `FunctionDecl`, `Program`

Uses the Visitor pattern for traversal.

### 4. Optimizer (`optimizer.h`, `optimizer.cpp`)
Three optimization passes:

1. **Constant Folding**: Evaluates constant expressions at compile time
2. **Dead Code Elimination**: Removes unused assignments and unreachable code
3. **Function Inlining**: Inlines small, non-recursive functions

### 5. Code Generator (`codegen.h`, `codegen.cpp`)
Generates stack-based bytecode from AST. Produces:
- Instruction stream
- Constant pool
- Function metadata table
- Incremental mode for REPL

**Scope Management**:
- Stack of scope maps for variable lookup
- Searches outer scopes for variable resolution
- Loop stack for break/continue handling

### 6. Virtual Machine (`vm.h`, `vm.cpp`)
Stack-based interpreter executing bytecode.

**Bytecode Format**:
- 1 byte: opcode
- 2 bytes: operand

**Value Types**:
- `void` (std::monostate)
- `int32_t` (integers)
- `std::string` (strings)
- `ArrayPtr` (shared_ptr to vector of Values)

**Opcodes**:
| Code | Name | Description |
|------|------|-------------|
| 0x00 | CONST | Push constant onto stack |
| 0x01 | LOAD | Load local variable |
| 0x02 | STORE | Store to local variable |
| 0x03 | ADD | Pop two, push sum |
| 0x04 | SUB | Pop two, push difference |
| 0x05 | MUL | Pop two, push product |
| 0x06 | DIV | Pop two, push quotient |
| 0x07 | MOD | Pop two, push remainder |
| 0x08 | JUMP | Unconditional jump |
| 0x09 | JUMP_IF_ZERO | Jump if top of stack is 0 |
| 0x0A | CALL | Call function |
| 0x0B | RETURN | Return from function |
| 0x0C | PRINT | Print top of stack |
| 0x0D | EQ | Equals comparison |
| 0x0E | NEQ | Not equals comparison |
| 0x0F | LT | Less than comparison |
| 0x10 | LTE | Less than or equal |
| 0x11 | GT | Greater than comparison |
| 0x12 | GTE | Greater than or equal |
| 0x13 | BUILD_ARRAY | Build array from N stack elements |
| 0x14 | ARRAY_LOAD | Load element from array |
| 0x15 | ARRAY_STORE | Store element to array |
| 0x16 | POP | Pop and discard top |
| 0x17 | NEG | Negate top of stack |
| 0x18 | NOT | Logical NOT |
| 0x19 | AND | Logical AND |
| 0x1A | OR | Logical OR |

### 7. REPL (`main.cpp`)
Interactive Read-Eval-Print Loop with:
- Persistent variable state across commands
- Incremental compilation
- Void result suppression
- Error recovery

### 8. Profiler (`profiler.h`)
Collects execution statistics:
- Opcode frequency counts
- Total instruction count
- Execution timing

## Data Structures

### Arrays
```javascript
let arr = [1, 2, 3];    // ArrayLiteralExpr → BUILD_ARRAY
arr[0]                   // IndexExpr → ARRAY_LOAD
arr[1] = 10              // ArrayAssignmentStmt → ARRAY_STORE
```

Arrays are reference types (shared_ptr), allowing nested arrays and mutation.

### Control Flow
```javascript
for (let i = 0; i < n; i = i + 1) {
    if (cond) { break; }    // Jumps to after loop
    if (other) { continue; } // Jumps to increment
    body();
}
```

Loop stack tracks break/continue jump targets for patch-up.

## Example Compilation

**Source**:
```javascript
let arr = [10, 20, 30];
let sum = 0;
for (let i = 0; i < 3; i = i + 1) {
    sum = sum + arr[i];
}
print(sum);
```

**Bytecode**:
```
[0]  CONST 0      ; Push 10
[1]  CONST 1      ; Push 20
[2]  CONST 2      ; Push 30
[3]  BUILD_ARRAY 3; Create [10, 20, 30]
[4]  STORE 0      ; arr = [10, 20, 30]
[5]  CONST 3      ; Push 0
[6]  STORE 1      ; sum = 0
[7]  CONST 3      ; Push 0
[8]  STORE 2      ; i = 0
[9]  LOAD 2       ; Push i (loop start)
[10] CONST 4      ; Push 3
[11] LT           ; i < 3?
[12] JUMP_IF_ZERO 22  ; Exit if false
[13] LOAD 1       ; Push sum
[14] LOAD 0       ; Push arr
[15] LOAD 2       ; Push i
[16] ARRAY_LOAD   ; arr[i]
[17] ADD          ; sum + arr[i]
[18] STORE 1      ; sum = result
[19] LOAD 2       ; Push i
[20] CONST 5      ; Push 1
[21] ADD          ; i + 1
[22] STORE 2      ; i = result
[23] JUMP 9       ; Loop back
[24] LOAD 1       ; Push sum
[25] PRINT        ; Output: 60
```

**Output**: `60`

## Memory Model

```
┌─────────────────────────────────────────────────────┐
│                    Constant Pool                     │
│  [0]=10  [1]=20  [2]=30  [3]=0  [4]=3  [5]=1        │
└─────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────┐
│                    Locals Array                      │
│  [0]=arr  [1]=sum  [2]=i  ...                       │
└─────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────┐
│                    Value Stack                       │
│  Operations push/pop values during execution         │
└─────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────┐
│                    Call Stack                        │
│  Tracks return addresses for function calls          │
└─────────────────────────────────────────────────────┘
```

## File Structure

```
ByteCode-Compiler/
├── include/
│   ├── common.h      # Types, opcodes, exceptions
│   ├── lexer.h       # Tokenizer
│   ├── parser.h      # Recursive descent parser
│   ├── ast.h         # AST node definitions
│   ├── codegen.h     # Bytecode generator
│   ├── optimizer.h   # Optimization passes
│   ├── vm.h          # Virtual machine
│   └── profiler.h    # Execution profiler
├── src/
│   ├── main.cpp      # CLI and REPL
│   ├── lexer.cpp
│   ├── parser.cpp
│   ├── ast.cpp
│   ├── codegen.cpp
│   ├── optimizer.cpp
│   └── vm.cpp
├── tests/            # 150+ GoogleTest cases
├── docs/             # Architecture and commit docs
├── examples/         # Demo programs
└── benchmarks/       # Performance testing
```
