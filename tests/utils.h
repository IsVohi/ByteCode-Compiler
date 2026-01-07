#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "codegen.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"
#include <sstream>
#include <string>

class TestUtils {
public:
  static std::string compileAndRun(const std::string &source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto program = parser.parseProgram();

    CodeGenerator codegen;
    auto bytecode = codegen.generate(*program);

    VirtualMachine vm;
    std::stringstream buffer;
    vm.setOutputStream(buffer);
    vm.execute(bytecode);
    return buffer.str();
  }
};

#endif // TEST_UTILS_H
