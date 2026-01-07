#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

#include "codegen.h"
#include "common.h"
#include "lexer.h"
#include "optimizer.h"
#include "parser.h"
#include "profiler.h"
#include "vm.h"

struct CompilerConfig {
  std::string input_file;
  bool optimize = true;
  bool profile = false;
  bool verbose = false;
  bool dumpBytecode = false;
};

/**
 * Read entire file contents into a string
 */
std::string readFile(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Cannot open file: " + path);
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

/**
 * Parse command-line arguments
 */
/**
 * Parse command-line arguments
 */
std::optional<CompilerConfig> parse_arguments(int argc, char *argv[]) {
  CompilerConfig config;

  if (argc < 2) {
    // REPL mode implied if no args
    return config;
  }

  std::string_view firstArg{argv[1]};
  if (firstArg.empty() || firstArg[0] == '-') {
    // No input file, just flags
  } else {
    config.input_file = argv[1];
  }

  for (int i = 1; i < argc; ++i) {
    std::string_view arg{argv[i]};
    if (arg == config.input_file)
      continue; // Skip input file if already handled

    if (arg == "--no-opt") {
      config.optimize = false;
    } else if (arg == "--profile") {
      config.profile = true;
    } else if (arg == "--verbose") {
      config.verbose = true;
    } else if (arg == "--dump") {
      config.dumpBytecode = true;
    } else {
      std::cerr << "Unknown flag: " << arg << "\n";
      return std::nullopt;
    }
  }

  return config;
}

void runRepl() {
  std::cout << "ByteCode Compiler REPL v1.0.0\n";
  std::cout << "Type 'exit' to quit.\n";

  std::string line;
  CodeGenerator codegen;
  VirtualMachine vm;
  Optimizer optimizer; // Optimizer might be tricky with incremental, maybe skip
                       // for REPL or verify safety

  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, line) || line == "exit") {
      break;
    }

    if (line.empty())
      continue;

    try {
      Lexer lexer(line);
      auto tokens = lexer.tokenize();

      Parser parser(tokens);
      auto program = parser.parseProgram();

      // Skip optimization in REPL for now to avoid complexity with incremental
      // state Optimizer optimizer; optimizer.run(*program);

      // Generate bytecode incrementally (preserve symbol tables)
      auto bytecode = codegen.generate(*program, true);

      // Execute incrementally (preserve stack/heap)
      Value result = vm.execute(bytecode, nullptr, true);

      // Only print non-void results
      if (!result.isVoid()) {
        if (result.isInt()) {
          std::cout << result.asInt() << std::endl;
        } else if (result.isString()) {
          std::cout << "\"" << result.asString() << "\"" << std::endl;
        } else if (result.isArray()) {
          std::cout << "[Array]" << std::endl;
        }
      }

    } catch (const std::exception &e) {
      std::cerr << "Error: " << e.what() << "\n";
    }
  }
}

int main(int argc, char *argv[]) {
  try {
    auto config = parse_arguments(argc, argv);
    if (!config) {
      return 1;
    }

    if (config->verbose) {
      std::cout << "=================================================\n";
      std::cout << "  Optimizing Bytecode Compiler v1.0.0\n";
      std::cout << "=================================================\n\n";
      std::cout << "Input file: " << config->input_file << "\n";
      std::cout << "Optimization: "
                << (config->optimize ? "enabled" : "disabled") << "\n";
      std::cout << "Profiling: " << (config->profile ? "enabled" : "disabled")
                << "\n\n";
    }

    if (config->input_file.empty()) {
      runRepl();
      return 0;
    }

    // Stage 1: Read source file
    if (config->verbose)
      std::cout << "[1/5] Reading source file...\n";
    std::string source = readFile(config->input_file);

    // Stage 2: Lexical analysis
    if (config->verbose)
      std::cout << "[2/5] Lexical analysis...\n";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    if (config->verbose) {
      std::cout << "      Generated " << tokens.size() << " tokens\n";
    }

    // Stage 3: Parsing
    if (config->verbose)
      std::cout << "[3/5] Parsing...\n";
    Parser parser(tokens);
    auto program = parser.parseProgram();
    if (config->verbose) {
      std::cout << "      AST with " << program->items().size()
                << " top-level items\n";
    }

    // Stage 4: Optimization (optional)
    if (config->optimize) {
      if (config->verbose)
        std::cout << "[4/5] Optimizing...\n";
      Optimizer optimizer;
      optimizer.run(*program);
      if (config->verbose) {
        auto stats = optimizer.getStats();
        std::cout << "      Constants folded: " << stats.constantsFolded
                  << "\n";
        std::cout << "      Dead code removed: " << stats.deadCodeRemoved
                  << "\n";
        std::cout << "      Functions inlinable: " << stats.functionsInlined
                  << "\n";
      }
    } else {
      if (config->verbose)
        std::cout << "[4/5] Skipping optimization\n";
    }

    // Stage 5: Code generation
    if (config->verbose)
      std::cout << "[5/5] Generating bytecode...\n";
    CodeGenerator codegen;
    auto bytecode = codegen.generate(*program);
    if (config->verbose) {
      std::cout << "      Generated " << bytecode.code.size()
                << " instructions\n";
      std::cout << "      Constants: " << bytecode.constants.size() << "\n";
      std::cout << "      Functions: " << bytecode.functions.size() << "\n";
    }

    if (config->dumpBytecode) {
      std::cout << "\n";
      bytecode.dump();
      std::cout << "\n";
    }

    // Stage 6: Execute
    if (config->verbose)
      std::cout << "\n--- Execution ---\n";

    VirtualMachine vm;
    Profiler profiler;

    if (config->profile) {
      profiler.startTiming();
    }

    Value result = vm.execute(bytecode, config->profile ? &profiler : nullptr);

    if (config->profile) {
      profiler.stopTiming();
    }

    if (config->verbose) {
      if (result.isInt()) {
        std::cout << "\n--- Result: " << result.asInt() << " ---\n";
      } else {
        std::cout << "\n--- Result: \"" << result.asString() << "\" ---\n";
      }
    }

    if (config->profile) {
      std::cout << "\n";
      profiler.dump();
    }

    return 0;

  } catch (const LexerError &e) {
    std::cerr << "Lexer error: " << e.what() << "\n";
    return 1;
  } catch (const ParserError &e) {
    std::cerr << "Parser error: " << e.what() << "\n";
    return 1;
  } catch (const CodegenError &e) {
    std::cerr << "Codegen error: " << e.what() << "\n";
    return 1;
  } catch (const VMError &e) {
    std::cerr << "Runtime error: " << e.what() << "\n";
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
}
