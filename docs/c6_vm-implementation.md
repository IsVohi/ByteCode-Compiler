# How I Did Commit 6 - Virtual Machine

## What Was the Goal of Commit 6?

The goal was to **execute bytecode**. We have:
1. Lexer → Tokens ✓
2. Parser → AST ✓
3. Code Generator → Bytecode ✓
4. **VM → Execution** ← This commit!

The Virtual Machine is where the rubber meets the road - it actually runs your program.

## What is a Stack-Based VM?

A stack-based VM uses a **stack** for all operations:

```
Code: 3 + 5

Stack operations:
Step 1: CONST 0    Stack: [3]
Step 2: CONST 1    Stack: [3, 5]
Step 3: ADD        Stack: [8]     (pop 3 and 5, push 3+5)
```

No registers needed - everything goes on the stack!

## How Does the VM Work?

### The Execution Loop

```cpp
Value VirtualMachine::execute(const BytecodeProgram& program) {
    while (ip < program.code.size()) {
        Instruction instr = program.code[ip];
        Opcode op = instr.opcode;
        uint16_t operand = instr.operand;
        
        switch (op) {
            case Opcode::CONST:    push(program.constants[operand]); break;
            case Opcode::ADD:      { auto b = pop(); auto a = pop(); push(a + b); } break;
            case Opcode::PRINT:    std::cout << pop() << std::endl; break;
            // ... handle all opcodes
        }
        
        ++ip;  // Move to next instruction
    }
    
    return stack_.empty() ? Value() : pop();
}
```

### Stack Operations

```cpp
void push(const Value& value) {
    if (stack_.size() >= MAX_STACK_SIZE) {
        throw VMError("Stack overflow");
    }
    stack_.push_back(value);
}

Value pop() {
    if (stack_.empty()) {
        throw VMError("Stack underflow");
    }
    Value value = std::move(stack_.back());
    stack_.pop_back();
    return value;
}
```

### Handling Each Opcode

#### Arithmetic (ADD, SUB, MUL, DIV, MOD)

```cpp
case Opcode::ADD: {
    Value b = pop();
    Value a = pop();
    push(Value(a.asInt() + b.asInt()));
    break;
}

case Opcode::DIV: {
    Value b = pop();
    Value a = pop();
    if (b.asInt() == 0) {
        throw VMError("Division by zero");
    }
    push(Value(a.asInt() / b.asInt()));
    break;
}
```

#### Variables (LOAD, STORE)

```cpp
case Opcode::LOAD: {
    uint16_t slot = basePointer + operand;
    push(locals_[slot]);
    break;
}

case Opcode::STORE: {
    Value value = pop();
    uint16_t slot = basePointer + operand;
    locals_[slot] = std::move(value);
    break;
}
```

#### Control Flow (JUMP, JUMP_IF_ZERO)

```cpp
case Opcode::JUMP: {
    ip = operand;  // Jump to address
    continue;      // Skip ip++ at end
}

case Opcode::JUMP_IF_ZERO: {
    Value condition = pop();
    if (condition.asInt() == 0) {
        ip = operand;  // Jump if false
        continue;
    }
    break;  // Otherwise continue
}
```

#### Function Calls (CALL, RETURN)

```cpp
case Opcode::CALL: {
    // Save return address
    callStack_.push_back({ip + 1, basePointer, argCount});
    
    // Jump to function
    ip = operand;
    basePointer = locals_.size() - argCount;
    continue;
}

case Opcode::RETURN: {
    Value returnValue = pop();
    
    // Restore caller's context
    auto frame = callStack_.back();
    callStack_.pop_back();
    
    ip = frame.returnAddress;
    basePointer = frame.basePointer;
    
    // Clean up locals and push return value
    locals_.resize(basePointer);
    push(returnValue);
    continue;
}
```

## The Call Stack

Functions need to track where to return:

```
main() calls add(3, 5)

Call Stack:
┌───────────────────┐
│ Return addr: 10   │  ← add() will return to instruction 10
│ Base pointer: 0   │
│ Arg count: 2      │
└───────────────────┘
```

When `add()` returns, we pop this frame and restore the caller's context.

## Memory Layout

```
Locals Array:
┌─────┬─────┬─────┬─────┬─────┬─────┐
│  x  │  y  │  z  │  a  │  b  │ ... │
└─────┴─────┴─────┴─────┴─────┴─────┘
  0     1     2     3     4
        ↑
        basePointer (for current function)

Stack:
┌─────┬─────┬─────┐
│  3  │  5  │  8  │
└─────┴─────┴─────┘
  Top →
```

## Complete Execution Example

**Source:**
```javascript
let x = 10;
let y = 20;
print(x + y);
```

**Bytecode:**
```
0: CONST 0    // Push 10
1: STORE 0    // x = 10
2: CONST 1    // Push 20
3: STORE 1    // y = 20
4: LOAD 0     // Push x
5: LOAD 1     // Push y
6: ADD        // x + y
7: PRINT      // Print result
```

**Execution Trace:**
```
IP=0  CONST 0     Stack: [10]       Locals: []
IP=1  STORE 0     Stack: []         Locals: [10]
IP=2  CONST 1     Stack: [20]       Locals: [10]
IP=3  STORE 1     Stack: []         Locals: [10, 20]
IP=4  LOAD 0      Stack: [10]       Locals: [10, 20]
IP=5  LOAD 1      Stack: [10, 20]   Locals: [10, 20]
IP=6  ADD         Stack: [30]       Locals: [10, 20]
IP=7  PRINT       Stack: []         Locals: [10, 20]   Output: 30
```

## Error Handling

The VM catches runtime errors:

```cpp
// Division by zero
if (b.asInt() == 0) {
    throw VMError("Division by zero");
}

// Stack overflow
if (stack_.size() >= MAX_STACK_SIZE) {
    throw VMError("Stack overflow");
}

// Undefined variable
if (slot >= locals_.size()) {
    throw VMError("Invalid local variable index");
}
```

## Files Created/Modified

| File | Lines | Description |
|------|-------|-------------|
| include/vm.h | 80+ | VirtualMachine class definition |
| src/vm.cpp | 350+ | VM implementation |
| tests/test_vm.cpp | 400+ | 30+ VM tests |

## Performance Considerations

- Direct switch dispatch (efficient)
- Move semantics for values (no copies)
- Pre-allocated stack space
- Simple instruction format

## Tests

```cpp
TEST(VMTest, ArithmeticOperations) {
    auto bytecode = compile("print(3 + 5 * 2);");
    VirtualMachine vm;
    vm.execute(bytecode);
    // Output: 13
}

TEST(VMTest, FunctionCall) {
    auto bytecode = compile(R"(
        fn add(a, b) { return a + b; }
        print(add(10, 20));
    )");
    VirtualMachine vm;
    vm.execute(bytecode);
    // Output: 30
}
```

## Summary

Commit 6 implements the Virtual Machine:
- ✅ Stack-based execution
- ✅ All 13 opcodes implemented
- ✅ Function calls with call stack
- ✅ Variable storage (locals)
- ✅ Error handling (division by zero, overflow)
- ✅ Passes 30+ tests

The compiler can now fully execute programs!

---

**Key Takeaway:** The VM is the runtime engine. It takes bytecode and makes things happen - variables get stored, math gets calculated, output gets printed.
