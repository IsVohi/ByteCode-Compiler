# How I Did Commit 8 - Control Flow (Loops, Break, Continue)

## What Was the Goal of Commit 8?

The goal was to implement **full control flow** - loops and the ability to break out of them or skip to the next iteration.

Think of it like:
1. **Basic VM** (Commit 6): Can execute straight-line code
2. **Control Flow** (Commit 8): Can loop, break, and continue

## What Did I Implement?

### 1. For Loops

**Syntax:**
```javascript
for (let i = 0; i < 10; i = i + 1) {
    print(i);
}
```

**How it compiles:**

```
Bytecode:
0: CONST 0         // Push 0
1: STORE 0         // i = 0
2: LOAD 0          // ← Loop starts here
3: CONST 1         // Push 10
4: COMPARE_LT      // i < 10?
5: JUMP_IF_ZERO 12 // If false, exit loop
6: LOAD 0          // Push i
7: PRINT           // Print i
8: LOAD 0          // Push i (for increment)
9: CONST 2         // Push 1
10: ADD            // i + 1
11: STORE 0        // i = i + 1
12: JUMP 2         // Jump back to condition
13: ...            // ← Loop exit
```

**Code Generator:**
```cpp
void CodeGenerator::visitForStmt(const ForStmt& stmt) {
    // New scope for loop variable
    scopes_.emplace_back();
    
    // 1. Init
    if (stmt.init()) {
        stmt.init()->accept(*this);
    }
    
    // 2. Loop start
    uint16_t loopStart = currentIndex();
    
    // 3. Condition
    size_t exitJump = -1;
    if (stmt.condition()) {
        stmt.condition()->accept(*this);
        exitJump = emitJump(Opcode::JUMP_IF_ZERO);
    }
    
    // 4. Body
    for (auto& s : stmt.body()) {
        s->accept(*this);
    }
    
    // 5. Increment
    if (stmt.increment()) {
        stmt.increment()->accept(*this);
    }
    
    // 6. Jump back
    emit(Opcode::JUMP, loopStart);
    
    // 7. Patch exit
    if (exitJump != (size_t)-1) {
        patchJump(exitJump, currentIndex());
    }
    
    scopes_.pop_back();
}
```

### 2. While Loops

**Syntax:**
```javascript
while (x > 0) {
    print(x);
    x = x - 1;
}
```

Compiles similarly to for loops, but simpler (no init or increment sections).

### 3. Break Statement

**Syntax:**
```javascript
for (let i = 0; i < 100; i = i + 1) {
    if (i == 5) {
        break;  // Exit the loop immediately
    }
    print(i);
}
// Prints: 0 1 2 3 4
```

**How break works:**

We maintain a **loop stack** to track where breaks should jump:

```cpp
struct LoopContext {
    std::vector<size_t> breakJumps;     // Addresses of break statements
    std::vector<size_t> continueJumps;  // Addresses of continue statements
    int continueTarget;                  // Where continue should jump
};

std::vector<LoopContext> loopStack_;
```

**When we see `break`:**
```cpp
void CodeGenerator::visitBreakStmt(const BreakStmt& stmt) {
    if (loopStack_.empty()) {
        throw CodegenError("Break statement outside of loop");
    }
    // Emit jump to unknown address, record it for patching later
    loopStack_.back().breakJumps.push_back(emitJump(Opcode::JUMP));
}
```

**At end of loop:**
```cpp
// Patch all breaks to point here (after the loop)
for (size_t offset : loopStack_.back().breakJumps) {
    patchJump(offset, currentIndex());
}
```

### 4. Continue Statement

**Syntax:**
```javascript
for (let i = 0; i < 5; i = i + 1) {
    if (i == 2) {
        continue;  // Skip to next iteration
    }
    print(i);
}
// Prints: 0 1 3 4 (skips 2)
```

**How continue works:**

Continue jumps to the **increment section**, not the start:

```cpp
void CodeGenerator::visitContinueStmt(const ContinueStmt& stmt) {
    if (loopStack_.empty()) {
        throw CodegenError("Continue statement outside of loop");
    }
    loopStack_.back().continueJumps.push_back(emitJump(Opcode::JUMP));
}
```

**At the increment section:**
```cpp
// Mark where continue should jump
loopStack_.back().continueTarget = currentIndex();

// After loop ends, patch all continues
for (size_t offset : loopStack_.back().continueJumps) {
    patchJump(offset, loopStack_.back().continueTarget);
}
```

## Complete Example

**Source:**
```javascript
for (let i = 0; i < 10; i = i + 1) {
    if (i == 3) { continue; }
    if (i == 7) { break; }
    print(i);
}
```

**Output:**
```
0
1
2
4
5
6
```

- When i=3: continue skips print, goes to i++
- When i=7: break exits the entire loop

## Nested Loops

Break and continue only affect the **innermost loop**:

```javascript
for (let i = 0; i < 3; i = i + 1) {
    for (let j = 0; j < 3; j = j + 1) {
        if (j == 1) { break; }  // Only breaks inner loop
        print(i * 10 + j);
    }
}
// Output: 0, 10, 20 (j never gets past 1)
```

This works because we use a **stack** of loop contexts:

```cpp
// Entering outer loop
loopStack_.push_back({});

// Entering inner loop  
loopStack_.push_back({});

// break; → affects loopStack_.back() (inner loop)

// Exiting inner loop
loopStack_.pop_back();

// Exiting outer loop
loopStack_.pop_back();
```

## Files Modified

| File | Changes |
|------|---------|
| include/ast.h | Added ForStmt, BreakStmt, ContinueStmt nodes |
| src/parser.cpp | Parse for, break, continue statements |
| src/codegen.cpp | Generate bytecode with loop tracking |
| tests/test_control_flow.cpp | 20+ tests for loops |

## Tests

```cpp
TEST(ControlFlowTest, ForLoop) {
    auto result = run("let sum = 0; for (let i = 0; i < 5; i = i + 1) { sum = sum + i; } print(sum);");
    EXPECT_EQ(output(), "10\n");  // 0+1+2+3+4 = 10
}

TEST(ControlFlowTest, BreakStatement) {
    auto result = run(R"(
        for (let i = 0; i < 10; i = i + 1) {
            if (i == 3) { break; }
            print(i);
        }
    )");
    EXPECT_EQ(output(), "0\n1\n2\n");
}

TEST(ControlFlowTest, ContinueStatement) {
    auto result = run(R"(
        for (let i = 0; i < 5; i = i + 1) {
            if (i == 2) { continue; }
            print(i);
        }
    )");
    EXPECT_EQ(output(), "0\n1\n3\n4\n");
}
```

## Summary

Commit 8 implements full control flow:
- ✅ For loops with init, condition, increment
- ✅ While loops
- ✅ Break statement (exits loop)
- ✅ Continue statement (skips to next iteration)
- ✅ Nested loop support
- ✅ Proper scope handling for loop variables
- ✅ Passes 20+ tests

The language now supports all common loop patterns!

---

**Key Takeaway:** Control flow is about managing the instruction pointer. Break and continue are just unconditional jumps that get patched to the right address after we know where the loop ends.
