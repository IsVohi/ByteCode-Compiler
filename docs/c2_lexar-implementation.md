# How I Did Commit 2 - Lexer Implementation

## What Is a Lexer? (Simple Explanation)

A **lexer** (also called tokenizer) is like a teacher reading a sentence and breaking it into words and punctuation.

**Example:**
```
Input:  "let x = 42;"
Lexer breaks it into:  [let] [x] [=] [42] [;]
```

Each piece (let, x, =, 42, ;) is called a **token**.

## What Was the Goal of Commit 2?

The goal was to implement the **Lexer** - the first stage of compilation. It takes raw source code and converts it into organized tokens that the next stage (Parser) can understand.

Think of it like:
```
Raw Code → Lexer → Organized Tokens → (Next: Parser will build structure)
```

## What Did I Create?

### 1. **include/lexer.h** - The Blueprint

This is the definition of the Lexer class:

```
What does it define?
- 30 different token types (like NUMBER, IDENTIFIER, KEYWORD_LET, etc.)
- A Token structure (contains type, text, line number, column number)
- The Lexer class methods
```

### 2. **src/lexer.cpp** - The Brain

This is where the actual work happens. It contains:

#### **Token Types (30 total)**
- **Keywords** (7): let, fn, if, else, while, return, print
- **Operators** (15): +, -, *, /, %, =, ==, !=, <, <=, >, >=, &&, ||, !
- **Delimiters** (6): (, ), {, }, ;, ,
- **Literals** (2): IDENTIFIER (names like x, sum), NUMBER (digits like 42)
- **Special** (2): END_OF_FILE, ILLEGAL

#### **How It Works**

The lexer reads the source code character by character:

1. **Skips whitespace** - Ignores spaces, tabs, newlines (they don't matter)
2. **Skips comments** - Ignores anything after // until end of line
3. **Recognizes patterns**:
   - If it sees letters, it's starting an identifier or keyword
   - If it sees numbers, it's a number
   - If it sees special symbols, it's an operator or delimiter
4. **Groups characters** - Like recognizing that == is one token, not two = tokens
5. **Tracks location** - Records line number and column for error messages
6. **Returns tokens** - Gives a list of all tokens found

#### **Key Methods**

- `tokenize()` - Main method that processes the entire source code
- `lexNumber()` - Handles numbers like 0, 42, 12345
- `lexIdentifierOrKeyword()` - Handles names and checks if they're keywords
- `lexOperatorOrDelimiter()` - Handles +, -, *, /, ==, etc.
- `skipWhitespaceAndComments()` - Ignores spaces and comments
- `currentChar()`, `peekChar()`, `advance()` - Helper methods to read characters

### 3. **tests/test_lexer.cpp** - The Proof It Works

This file has **50+ test cases** that verify the lexer works correctly:

#### **Test Categories**

1. **Number Tests** (4 tests)
   - Can it recognize 0? ✓
   - Can it recognize 42? ✓
   - Can it recognize 12345? ✓

2. **Identifier Tests** (5 tests)
   - Can it recognize x? ✓
   - Can it recognize _hidden? ✓
   - Can it recognize factorial1? ✓

3. **Keyword Tests** (7 tests)
   - Can it recognize all 7 keywords? ✓

4. **Operator Tests** (15 tests)
   - Can it recognize +, -, *, /, %, =? ✓
   - Can it recognize ==, !=, <=, >=? ✓
   - Can it recognize &&, ||? ✓

5. **Delimiter Tests** (6 tests)
   - Can it recognize (, ), {, }, ;, ,? ✓

6. **Whitespace Tests** (4 tests)
   - Can it skip spaces, tabs, newlines? ✓

7. **Comment Tests** (3 tests)
   - Can it skip // comments? ✓

8. **Complex Program Tests** (5 tests)
   - Can it tokenize a complete function? ✓
   - Can it handle mixed operators? ✓

9. **Error Tests** (4 tests)
   - Does it reject illegal characters? ✓

10. **Edge Case Tests** (5 tests)
    - Empty source? ✓
    - Only whitespace? ✓
    - Very long identifiers? ✓

**Result: All 50+ tests pass!** ✓

### 4. **CMakeLists.txt** - Updated Build Instructions

I updated the build file to include:
- Added `src/lexer.cpp` to the compiler
- Added `tests/test_lexer.cpp` to the tests
- Made sure lexer tests run with the test system

## How Does It Work? (Step by Step)

**Example: Tokenizing `let x = 42;`**

```
Step 1: Read first character 'l'
        It's a letter, so start an identifier
        Read: l-e-t
        Check: is "let" a keyword? YES
        Create token: [KW_LET, "let", line 1, col 1]

Step 2: Skip space

Step 3: Read character 'x'
        It's a letter, so start an identifier
        Read: x
        Check: is "x" a keyword? NO
        Create token: [IDENTIFIER, "x", line 1, col 5]

Step 4: Skip space

Step 5: Read character '='
        It's an operator
        Check next character: not '='
        Create token: [ASSIGN, "=", line 1, col 7]

Step 6: Skip space

Step 7: Read character '4'
        It's a digit, so start a number
        Read: 4-2
        Create token: [NUMBER, "42", line 1, col 9]

Step 8: Read character ';'
        It's a delimiter
        Create token: [SEMICOLON, ";", line 1, col 11]

Step 9: End of file
        Create token: [END_OF_FILE, "", line 1, col 12]
```

**Output: 6 tokens**
```
[KW_LET] [IDENTIFIER] [ASSIGN] [NUMBER] [SEMICOLON] [EOF]
```

## Files I Created/Modified

### New Files (3)
- **include/lexer.h** (128 lines) - The class definition
- **src/lexer.cpp** (350+ lines) - The implementation
- **tests/test_lexer.cpp** (500+ lines) - 50+ tests

### Modified File (1)
- **CMakeLists.txt** - Updated to include lexer in build

## How to Use It

### Build
```bash
cd compiler
mkdir -p build
cd build
cmake ..
make
```

### Run Tests
```bash
make test_all
```

### Expected Output
```
Running 50+ tests...
All tests passed! ✓
0 tests failed
```

## Key Features

✅ **30 different token types** - Covers all language constructs
✅ **Line & column tracking** - Knows exactly where in the file each token is
✅ **Whitespace handling** - Automatically skips spaces, tabs, newlines
✅ **Comment handling** - Ignores // comments automatically
✅ **Multi-char operators** - Recognizes == as one token, not two = tokens
✅ **Error messages** - If it finds illegal characters, it tells you exactly where
✅ **50+ tests** - Thoroughly tested and verified

## Why Is This Important?

The lexer is the **first step** in compilation:
1. **Lexer** (Commit 2) - Breaks code into tokens ✓
2. **Parser** (Commit 3) - Arranges tokens into structure
3. **Code Generator** (Commit 4) - Converts to bytecode
4. **Optimizer** (Commit 5) - Makes bytecode faster
5. **Virtual Machine** (Commit 6) - Executes bytecode

Without the lexer, we can't understand the source code at all!

## Code Quality

✅ **Zero warnings** - Builds clean
✅ **Zero errors** - No compilation problems
✅ **Well documented** - Comments explain what each part does
✅ **Well tested** - 50+ tests verify it works
✅ **Production ready** - Could be used in a real compiler

## How Many Lines of Code?

- **lexer.h**: 128 lines (definitions)
- **lexer.cpp**: 350+ lines (implementation)
- **test_lexer.cpp**: 500+ lines (tests)
- **Total**: 1000+ lines of code + documentation

## Summary

Commit 2 implements the **tokenizer** - the first stage that reads source code and breaks it into meaningful pieces (tokens). It:
- Recognizes 30 different token types
- Skips whitespace and comments
- Tracks line/column positions
- Handles multi-character operators
- Provides helpful error messages
- Passes all 50+ tests

The lexer is now ready to pass its tokens to the Parser in the next commit!