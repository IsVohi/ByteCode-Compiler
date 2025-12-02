#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

#include "common.h"
struct CompilerConfig {
    std::string input_file;
    bool optimize = true;
    bool profile = false;
    bool verbose = false;
};


// CLI Parser


/**
 * Parse command-line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @return Optional CompilerConfig if parsing succeeds
 */
std::optional<CompilerConfig> parse_arguments(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: compiler <input_file> [--no-opt] [--profile] [--verbose]\n";
        return std::nullopt;
    }

    CompilerConfig config;
    config.input_file = argv[1];

    // Parse flags
    for (int i = 2; i < argc; ++i) {
        std::string_view arg{argv[i]};
        if (arg == "--no-opt") {
            config.optimize = false;
        } else if (arg == "--profile") {
            config.profile = true;
        } else if (arg == "--verbose") {
            config.verbose = true;
        } else {
            std::cerr << "Unknown flag: " << arg << "\n";
            return std::nullopt;
        }
    }

    return config;
}

// ============================================================================
// Main Entry Point
// ============================================================================

int main(int argc, char* argv[]) {
    try {
        // Parse command-line arguments
        auto config = parse_arguments(argc, argv);
        if (!config) {
            return 1;
        }

        // Print welcome message
        std::cout << "=================================================\n";
        std::cout << "  Optimizing Bytecode Compiler v0.1.0\n";
        std::cout << "=================================================\n";
        std::cout << "\n";

        // Display configuration
        std::cout << "Configuration:\n";
        std::cout << "  Input file:       " << config->input_file << "\n";
        std::cout << "  Optimization:     " << (config->optimize ? "enabled" : "disabled") << "\n";
        std::cout << "  Profiling:        " << (config->profile ? "enabled" : "disabled") << "\n";
        std::cout << "  Verbose output:   " << (config->verbose ? "enabled" : "disabled") << "\n";
        std::cout << "\n";

        // Print bytecode information
        std::cout << "Bytecode Information:\n";
        std::cout << "  Version:          " << static_cast<int>(BYTECODE_VERSION) << "\n";
        std::cout << "  Max stack size:   " << MAX_STACK_SIZE << "\n";
        std::cout << "  Max variables:    " << MAX_VARIABLES << "\n";
        std::cout << "  Max instructions: " << MAX_INSTRUCTIONS << "\n";
        std::cout << "\n";

        // Print available opcodes
        std::cout << "Available Opcodes:\n";
        std::cout << "  0x00 - " << opcode_to_string(Opcode::CONST) << "\n";
        std::cout << "  0x01 - " << opcode_to_string(Opcode::LOAD) << "\n";
        std::cout << "  0x02 - " << opcode_to_string(Opcode::STORE) << "\n";
        std::cout << "  0x03 - " << opcode_to_string(Opcode::ADD) << "\n";
        std::cout << "  0x04 - " << opcode_to_string(Opcode::SUB) << "\n";
        std::cout << "  0x05 - " << opcode_to_string(Opcode::MUL) << "\n";
        std::cout << "  0x06 - " << opcode_to_string(Opcode::DIV) << "\n";
        std::cout << "  0x07 - " << opcode_to_string(Opcode::MOD) << "\n";
        std::cout << "  0x08 - " << opcode_to_string(Opcode::JUMP) << "\n";
        std::cout << "  0x09 - " << opcode_to_string(Opcode::JUMP_IF_ZERO) << "\n";
        std::cout << "  0x0A - " << opcode_to_string(Opcode::CALL) << "\n";
        std::cout << "  0x0B - " << opcode_to_string(Opcode::RETURN) << "\n";
        std::cout << "  0x0C - " << opcode_to_string(Opcode::PRINT) << "\n";
        std::cout << "\n";

        // Status message
        std::cout << "[Compilation pipeline not yet implemented]\n";
        std::cout << "\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
}
