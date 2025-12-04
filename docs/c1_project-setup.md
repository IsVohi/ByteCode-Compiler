# How I Did Commit 1 - Project Setup

## What Was the Goal?

The goal of the first commit was to set up the basic foundation for the compiler project. Think of it like building the walls and floor of a house before adding furniture.

## What Did I Create?

### 1. The Project Structure (Folders)

I created these folders:
- **src/** - for source code files
- **include/** - for header files (.h files)
- **tests/** - for test files
- **benchmarks/** - for performance testing (empty for now)
- **results/** - for storing output results (empty for now)

### 2. The Main Files

#### **include/common.h** - The Rulebook
This file contains all the rules and definitions that the entire compiler will use:
- **13 Bytecode Opcodes** - The basic instructions like ADD, SUB, LOAD, STORE, etc.
- **Exception Classes** - Different types of errors (LexerError, ParserError, etc.)
- **Constants** - System limits like max stack size (256), max variables (1024)
- **Helper Function** - A function to convert opcodes to readable names

Think of this like a dictionary that everyone in the project refers to.

#### **src/main.cpp** - The Starting Point
This is the main entry point of the compiler. It:
- Reads command-line arguments (like flags: `--no-opt`, `--profile`, `--verbose`)
- Prints a welcome message showing the compiler version
- Shows bytecode information (how many opcodes, max stack size, etc.)
- Lists all 13 available opcodes
- Says "[Compilation pipeline not yet implemented]" (because we haven't built it yet)

It's like the front desk of a hotel - it receives visitors and tells them what's available.

#### **tests/test_runner.cpp** - The Testing Setup
This file sets up Google Test framework:
- It's mostly empty with just one placeholder test
- It proves that the testing system works
- Later commits will add real tests here

#### **CMakeLists.txt** - The Build Instructions
This tells the computer how to compile the project:
- Requires C++17 (modern C++)
- Turns warnings into errors (strict code quality)
- Creates the main `compiler` executable
- Sets up the test runner
- Fetches Google Test automatically

It's like a recipe for baking - it says what ingredients you need and in what order.

#### **.gitignore** - What NOT to Save
This file tells Git (version control) what files to ignore:
- Build folders (build/, CMakeFiles/)
- IDE files (.vscode/, .idea/)
- Compiled files (*.o, *.exe)
- Temporary files
- Test results

It keeps your repository clean.

#### **README.md** - The Documentation
This is the project documentation that explains:
- What the project does
- How to build it
- The architecture (how everything fits together)
- Bytecode specification (what opcodes do)
- Next steps

It's the instruction manual.

## How Did I Build It?

```
1. Created folder structure
2. Wrote the header file (common.h)
3. Wrote the CLI entry point (main.cpp)
4. Set up test framework (test_runner.cpp)
5. Created build instructions (CMakeLists.txt)
6. Created git ignore rules (.gitignore)
7. Wrote documentation (README.md)
8. Committed everything to Git
```

## What Does It Do When You Run It?

```bash
./compiler test.src
```

Output:
```
==================================================
  Optimizing Bytecode Compiler v0.1.0
==================================================

Configuration:
  Input file:       test.src
  Optimization:     enabled
  Profiling:        disabled
  Verbose output:   disabled

Bytecode Information:
  Version:          1
  Max stack size:   256
  Max variables:    1024
  Max instructions: 65535

Available Opcodes:
  0x00 - CONST
  0x01 - LOAD
  0x02 - STORE
  ... (all 13 opcodes listed)
  0x0C - PRINT

[Compilation pipeline not yet implemented]
```

## How Many Lines of Code?

- **common.h**: 128 lines
- **main.cpp**: 99 lines
- **test_runner.cpp**: 26 lines
- **CMakeLists.txt**: 54 lines
- **Total**: About 350 lines of code + documentation

## Key Points

✅ **No compilation logic yet** - It's just a skeleton
✅ **All opcodes defined** - 13 bytecode instructions ready
✅ **Exception system ready** - Error handling structure in place
✅ **Build system working** - CMake configured and ready
✅ **Tests set up** - GoogleTest integrated
✅ **Well documented** - README and comments explain everything

## Why Is This Important?

This first commit is the **foundation**. Everything else will be built on top of it:
- Next commit will add the **Lexer** (breaks code into tokens)
- Future commits will add **Parser**, **Code Generator**, **Optimizer**, **Virtual Machine**

It's like building the foundation of a house - you need it solid before adding walls!

## Summary

The first commit is basically saying: "Here's the basic structure, here's how the build system works, here's what opcodes we support, and here's how to test it. Now we're ready to start actually implementing the compiler!"
