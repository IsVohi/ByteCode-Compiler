# How I Did Commit 9 - Arrays & REPL

## What Was the Goal of Commit 9?

The goal was to add **data structures (arrays)** and an **interactive REPL** mode. This makes the language much more practical.

## Part 1: Arrays

### Array Syntax

```javascript
// Array literals
let numbers = [1, 2, 3, 4, 5];

// Array indexing
print(numbers[0]);   // 1
print(numbers[2]);   // 3

// Array assignment
numbers[1] = 99;
print(numbers);      // [1, 99, 3, 4, 5]

// Nested arrays
let matrix = [[1, 2], [3, 4]];
print(matrix[0][1]); // 2
```

### New AST Nodes

```cpp
// Array literal: [1, 2, 3]
class ArrayLiteralExpr : public Expr {
    std::vector<std::unique_ptr<Expr>> elements_;
};

// Index expression: arr[i]
class IndexExpr : public Expr {
    std::unique_ptr<Expr> target_;  // The array
    std::unique_ptr<Expr> index_;   // The index
};

// Array assignment: arr[i] = value
class ArrayAssignmentStmt : public Stmt {
    std::unique_ptr<Expr> target_;
    std::unique_ptr<Expr> index_;
    std::unique_ptr<Expr> value_;
};
```

### New Opcodes

| Opcode | Description |
|--------|-------------|
| BUILD_ARRAY | Pop N elements, build array |
| ARRAY_LOAD | Pop index, pop array, push element |
| ARRAY_STORE | Pop value, pop index, pop array, store |

### Code Generation

**Array literal:**
```cpp
void CodeGenerator::visitArrayLiteralExpr(const ArrayLiteralExpr& expr) {
    // Push all elements
    for (auto& element : expr.elements()) {
        element->accept(*this);
    }
    // Build array from top N stack elements
    emit(Opcode::BUILD_ARRAY, expr.elements().size());
}
```

**Array indexing:**
```cpp
void CodeGenerator::visitIndexExpr(const IndexExpr& expr) {
    expr.target().accept(*this);  // Push array
    expr.index().accept(*this);   // Push index
    emit(Opcode::ARRAY_LOAD);     // Pop both, push element
}
```

### VM Implementation

**Array type:**
```cpp
using ArrayPtr = std::shared_ptr<std::vector<Value>>;

struct Value {
    std::variant<std::monostate, int32_t, std::string, ArrayPtr> data;
    
    bool isArray() const { return std::holds_alternative<ArrayPtr>(data); }
    ArrayPtr asArray() const { return std::get<ArrayPtr>(data); }
};
```

**BUILD_ARRAY opcode:**
```cpp
case Opcode::BUILD_ARRAY: {
    auto arr = std::make_shared<std::vector<Value>>();
    for (uint16_t i = 0; i < operand; ++i) {
        arr->push_back(pop());
    }
    std::reverse(arr->begin(), arr->end());
    push(Value(arr));
    break;
}
```

**ARRAY_LOAD opcode:**
```cpp
case Opcode::ARRAY_LOAD: {
    Value indexVal = pop();
    Value arrayVal = pop();
    
    auto arr = arrayVal.asArray();
    int index = indexVal.asInt();
    
    if (index < 0 || index >= arr->size()) {
        throw VMError("Array index out of bounds");
    }
    
    push((*arr)[index]);
    break;
}
```

### Array Printing

Arrays print their contents recursively:

```javascript
let arr = [1, [2, 3], 4];
print(arr);  // [1, [2, 3], 4]
```

```cpp
void VirtualMachine::printValue(const Value& value, std::ostream& os) const {
    if (value.isInt()) {
        os << value.asInt();
    } else if (value.isArray()) {
        os << "[";
        auto arr = value.asArray();
        for (size_t i = 0; i < arr->size(); ++i) {
            if (i > 0) os << ", ";
            printValue((*arr)[i], os);  // Recursive!
        }
        os << "]";
    }
}
```

## Part 2: REPL

### What is a REPL?

**R**ead-**E**valuate-**P**rint **L**oop - an interactive mode where you type code and see results immediately.

```
$ ./compiler
ByteCode Compiler REPL v1.0.0
Type 'exit' to quit.
> let x = 5;
> let y = 10;
> print(x + y);
15
> exit
```

### Persistent State

The key feature is that **variables persist between commands**:

```
> let x = 10;
> x = x + 5;
> print(x);
15
```

This requires keeping the CodeGenerator and VM state across invocations.

### REPL Implementation

```cpp
void repl() {
    CodeGenerator codegen;  // Keeps symbol tables
    VirtualMachine vm;      // Keeps locals and stack
    
    std::cout << "ByteCode Compiler REPL v1.0.0\n";
    std::cout << "Type 'exit' to quit.\n";
    
    std::string line;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, line);
        
        if (line == "exit") break;
        if (line.empty()) continue;
        
        try {
            Lexer lexer(line);
            auto tokens = lexer.tokenize();
            
            Parser parser(tokens);
            auto program = parser.parseProgram();
            
            // Generate bytecode incrementally
            auto bytecode = codegen.generate(*program, true);  // incremental=true
            
            // Execute incrementally
            Value result = vm.execute(bytecode, nullptr, true);
            
            // Print result if not void
            if (!result.isVoid()) {
                printValue(result, std::cout);
                std::cout << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
```

### Incremental Mode

The `incremental=true` flag tells codegen and VM to:

1. **CodeGenerator**: Don't reset symbol tables, keep variable mappings
2. **VM**: Don't reset locals, keep variable values

```cpp
BytecodeProgram CodeGenerator::generate(const Program& program, bool incremental) {
    if (!incremental) {
        // Fresh compilation - reset everything
        program_ = BytecodeProgram{};
        scopes_.clear();
        scopes_.emplace_back();  // Global scope
    }
    // ... generate code
}
```

### Void Type for Clean Output

We added a `std::monostate` to the Value variant to represent "void" - statements that don't produce a value:

```cpp
// Old behavior:
> let x = 5;
0              // Spurious output

// New behavior:
> let x = 5;
>              // No output for declarations
> x + 10;
15             // Expressions show their value
```

```cpp
if (!result.isVoid()) {
    // Only print if there's actually a value
    printValue(result, std::cout);
}
```

### REPL Features

✅ Persistent variables
✅ Multi-line friendly
✅ Error recovery (one bad line doesn't crash REPL)
✅ Clean output (no spurious zeros)
✅ Exit command

## Files Modified

| File | Changes |
|------|---------|
| include/ast.h | ArrayLiteralExpr, IndexExpr, ArrayAssignmentStmt |
| include/common.h | ArrayPtr type, monostate for void |
| src/parser.cpp | Parse array literals and indexing |
| src/codegen.cpp | Generate BUILD_ARRAY, ARRAY_LOAD, ARRAY_STORE |
| src/vm.cpp | Execute array opcodes, printValue helper |
| src/main.cpp | REPL loop implementation |
| tests/test_arrays.cpp | 15+ array tests |

## Tests

```cpp
TEST(ArrayTest, ArrayLiteral) {
    auto output = run("print([1, 2, 3]);");
    EXPECT_EQ(output, "[1, 2, 3]\n");
}

TEST(ArrayTest, ArrayIndexing) {
    auto output = run("let arr = [10, 20, 30]; print(arr[1]);");
    EXPECT_EQ(output, "20\n");
}

TEST(ArrayTest, ArrayAssignment) {
    auto output = run(R"(
        let arr = [1, 2, 3];
        arr[0] = 99;
        print(arr);
    )");
    EXPECT_EQ(output, "[99, 2, 3]\n");
}

TEST(ArrayTest, NestedArrays) {
    auto output = run("let m = [[1, 2], [3, 4]]; print(m[1][0]);");
    EXPECT_EQ(output, "3\n");
}
```

## Summary

Commit 9 adds arrays and REPL:

**Arrays:**
- ✅ Array literal syntax: `[1, 2, 3]`
- ✅ Array indexing: `arr[i]`
- ✅ Array assignment: `arr[i] = value`
- ✅ Nested arrays: `matrix[0][1]`
- ✅ Recursive printing: `[1, [2, 3], 4]`
- ✅ Bounds checking with errors

**REPL:**
- ✅ Persistent state between commands
- ✅ Incremental compilation
- ✅ Clean output (void suppression)
- ✅ Error recovery
- ✅ Exit command

The language is now fully interactive and supports data structures!

---

**Key Takeaway:** Arrays are the first complex data type - they require shared pointers for proper memory management. The REPL makes development much more convenient by keeping state between commands.
