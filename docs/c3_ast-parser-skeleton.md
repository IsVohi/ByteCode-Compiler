# How I Did Commit 3 - AST & Parser Skeleton

## What Was the Goal of Commit 3?

The goal was to **set up the blueprint for parsing** - define what the Abstract Syntax Tree (AST) looks like and create the Parser class structure. But don't actually implement the parsing logic yet - just the framework.

Think of it like:
1. **Lexer** (Commit 2): Breaks code into tokens
2. **AST & Parser Skeleton** (Commit 3): Prepares the structure
3. **Full Parser** (Commit 4): Actually builds the tree

## What is an AST?

An **AST (Abstract Syntax Tree)** is a tree structure that represents your source code in a way the compiler understands.

**Example:**
```
Code:  let x = 42;

AST:
    AssignmentStmt
    ├── name: "x"
    └── value: NumberExpr(42)
```

Another example:
```
Code:  a + b * c

AST:
      BinaryOpExpr(+)
      ├── left: IdentifierExpr("a")
      └── right: BinaryOpExpr(*)
                 ├── left: IdentifierExpr("b")
                 └── right: IdentifierExpr("c")
```

## What Did I Create?

### 1. **include/ast.h** - The Node Definitions (400+ lines)

This file defines 12 types of AST nodes:

#### **Expression Nodes (6 types)**
```
NumberExpr        // 42, 0, 999
IdentifierExpr    // x, sum, factorial
BinaryOpExpr      // a + b, x <= y, left && right
UnaryOpExpr       // -x, !condition
FunctionCallExpr  // add(3, 5), factorial(n)
```

Each node stores its data. For example:
```cpp
class NumberExpr {
    int value_;  // Stores the number
};

class BinaryOpExpr {
    Expr* left_;       // Left side expression
    Operator op_;      // The operator (+ - * / etc)
    Expr* right_;      // Right side expression
};
```

#### **Statement Nodes (6 types)**
```
AssignmentStmt    // let x = 42;
PrintStmt         // print(x);
IfStmt            // if (cond) { body }
WhileStmt         // while (cond) { body }
ReturnStmt        // return expr;
BlockStmt         // { stmt* }
```

#### **Top-Level Nodes (2 types)**
```
FunctionDecl      // fn name(params) { body }
Program           // Root: entire program
```

### 2. **The Visitor Pattern**

The **visitor pattern** is a design pattern that lets you do operations on the AST without changing the node classes themselves.

**Simple analogy:** Imagine a tree of people (the AST). You want to:
- Count how many people
- Calculate average height
- Print everyone's names

With the visitor pattern, you can write different "visitors" that walk the tree:

```cpp
class CountVisitor : public ASTVisitor {
    // Counts nodes
};

class PrintVisitor : public ASTVisitor {
    // Prints nodes
};

// Use them:
ast_tree->accept(CountVisitor());  // Count nodes
ast_tree->accept(PrintVisitor());  // Print nodes
```

Each node has an `accept()` method that "accepts" a visitor:

```cpp
class NumberExpr : public Expr {
    void accept(ASTVisitor& visitor) const override {
        visitor.visitNumberExpr(*this);
    }
};
```

When you call `accept()`, it tells the visitor "hey, you have a NumberExpr to process!"

This is clean because:
- Adding new operations (code generation, optimization) doesn't require changing AST nodes
- Each operation (visitor) is separate
- Easy to understand and extend

### 3. **include/parser.h** - The Parser Interface (300+ lines)

This file defines the Parser class - what methods it has and what they do.

#### **Main Entry Points**
```cpp
parseProgram()      // Parse entire source file
parseFunction()     // Parse a function
parseStatement()    // Parse one statement
parseExpression()   // Parse an expression
```

#### **Expression Parsing Methods** (in order of precedence)
```cpp
parseExpression()    // Entry point for expressions
parseLogicalOr()     // ||
parseLogicalAnd()    // &&
parseComparison()    // ==, !=
parseRelational()    // <, <=, >, >=
parseTerm()          // +, -
parseFactor()        // *, /, %
parseUnary()         // -, !
parsePrimary()       // Numbers, identifiers, calls, parentheses
```

The order matters! It's called **operator precedence**. For example:
- `*` has higher precedence than `+`
- `!` has higher precedence than `&&`
- This is why `3 + 5 * 2 = 13` (not 16)

#### **Helper Methods**
```cpp
currentToken()   // Get current token
advance()        // Move to next token
check()          // Does current token match type?
expect()         // Expect specific token or error
```

#### **Error Handling**
```cpp
error()          // Report error with location
errorAt()        // Report "expected X but found Y"
formatError()    // Add line/column info
```

### 4. **src/parser.cpp** - The Skeleton Implementation (200+ lines)

All the methods are declared but **throw "not implemented"**:

```cpp
std::unique_ptr<Program> Parser::parseProgram() {
    throw ParserError("Parsing not yet implemented");
}

std::unique_ptr<Expr> Parser::parseExpression() {
    throw ParserError("Parsing not yet implemented");
}

// etc for all 15+ methods
```

But the **helper methods ARE implemented**:
- `currentToken()` - returns current token
- `advance()` - moves to next token
- `isAtEnd()` - checks if done
- `expect()` - checks token type
- `error()` - throws error with location

These are ready to use for Commit 4!

### 5. **src/ast.cpp** - Visitor Pattern Implementation (60 lines)

Just implements the `accept()` methods:

```cpp
void NumberExpr::accept(ASTVisitor& visitor) const {
    visitor.visitNumberExpr(*this);
}

void BinaryOpExpr::accept(ASTVisitor& visitor) const {
    visitor.visitBinaryOpExpr(*this);
}

// ... one for each node type
```

Each one simply calls the appropriate visitor method.

### 6. **tests/test_parser.cpp** - 20+ Tests (350+ lines)

Test groups:

1. **Parser Construction** (3 tests)
   - Can we create a parser? ✓
   - Can it handle empty tokens? ✓

2. **AST Node Creation** (10 tests)
   - Can we create NumberExpr? ✓
   - Can we create BinaryOpExpr? ✓
   - Can we create all 12 node types? ✓

3. **Visitor Pattern** (3 tests)
   - Does visitor dispatch work? ✓

4. **Parser Methods** (5 tests)
   - Do parsing methods throw expected errors? ✓

5. **Memory Management** (2 tests)
   - No memory leaks? ✓

All tests compile and pass!

## How It All Works Together

```
Input: let x = 42;

Step 1 - Lexer (Commit 2):
  Produces: [KW_LET] [IDENTIFIER(x)] [ASSIGN] [NUMBER(42)] [SEMICOLON] [EOF]

Step 2 - Parser Constructor (Commit 3):
  Parser receives token stream

Step 3 - parseProgram() (Commit 4 - not yet):
  Would parse tokens into AST

Step 4 - AST Visitor (Commit 3 ready, Commit 5+ uses):
  Could walk tree with CodegenVisitor, OptimizeVisitor, etc.
```

## File Sizes & Statistics

| File | Lines | What It Does |
|------|-------|--------------|
| ast.h | 400+ | 12 AST node definitions |
| ast.cpp | 60 | Visitor accept() implementations |
| parser.h | 300+ | Parser interface with docs |
| parser.cpp | 200+ | Parser skeleton |
| test_parser.cpp | 350+ | 20+ test cases |
| **Total** | **1300+** | **AST infrastructure** |

## Key Design Decisions

### Why Visitor Pattern?

**Good for:**
- Adding new operations (codegen, optimizer) without modifying nodes
- Clean separation of concerns
- Easy to understand
- Used in real compilers (LLVM, GCC, etc.)

**Trade-off:**
- Slightly more complex than simple node methods
- But worth it for a real compiler

### Why Unique Pointers?

**Good for:**
- Automatic memory management
- No leaks (tested!)
- Clear ownership
- Exception safe

**Trade-off:**
- Can't share nodes (intentional - AST is a tree)
- Need `std::move` when constructing

### Why Operator Enums?

```cpp
enum class Operator {
    PLUS, MINUS, MULTIPLY, DIVIDE, MODULO,
    EQ, NEQ, LT, LTE, GT, GTE,
    AND, OR
};
```

**Good for:**
- Type-safe operators
- No string comparisons
- Easy to enumerate all operators

**Trade-off:**
- Must list all operators upfront
- Adding new operators requires changes

## What Happens in Commit 4

Right now (Commit 3):
```cpp
Parser parser(tokens);
parser.parseProgram();  // Throws "not implemented"
```

In Commit 4:
```cpp
Parser parser(tokens);
auto ast = parser.parseProgram();  // Actually works!
// Returns a tree of AST nodes
```

The parsing logic will be implemented - the whole precedence climbing, recursive descent, everything!

## Why This Matters

Commit 3 provides the **infrastructure** for parsing:
- ✅ Define what we want to build (AST nodes)
- ✅ Define how we'll work with it (visitor pattern)
- ✅ Define the interface (parser methods)
- ✅ Set up testing framework

Commit 4 will provide the **implementation**:
- Actual parsing logic
- Precedence climbing
- Error recovery
- Real AST creation

## Summary

Commit 3 is like building a **blueprint for a house**:
- Defines what rooms you need (AST nodes)
- Defines how they connect (visitor pattern)
- Defines the interface (parser methods)
- Doesn't build the actual house yet

Commit 4 will build it. But without Commit 3, Commit 4 would have nowhere to build!

Both are essential. Commit 3 is the careful planning. Commit 4 is the actual construction.

---

**Key Takeaway:** Commit 3 sets up everything needed for parsing. All skeleton is in place. Ready for real implementation in Commit 4.
