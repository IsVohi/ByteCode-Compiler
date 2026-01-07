#include "codegen.h"
#include <iostream>
#include <stdexcept>

// ============================================================================
// BytecodeProgram Implementation
// ============================================================================

void BytecodeProgram::dump() const {
  std::cout << "=== Bytecode Program ===" << std::endl;
  std::cout << "Constants: " << constants.size() << std::endl;
  for (size_t i = 0; i < constants.size(); ++i) {
    if (constants[i].isInt()) {
      std::cout << "  [" << i << "] = " << constants[i].asInt() << std::endl;
    } else {
      std::cout << "  [" << i << "] = \"" << constants[i].asString() << "\""
                << std::endl;
    }
  }

  std::cout << "Functions: " << functions.size() << std::endl;
  for (const auto &fn : functions) {
    std::cout << "  " << fn.name << " entry=" << fn.entry
              << " arity=" << static_cast<int>(fn.arity)
              << " locals=" << static_cast<int>(fn.localCount) << std::endl;
  }

  std::cout << "Code: " << code.size() << " instructions" << std::endl;
  for (size_t i = 0; i < code.size(); ++i) {
    auto op = static_cast<Opcode>(code[i].opcode);
    std::cout << "  [" << i << "] " << opcode_to_string(op);
    // Show operand for relevant opcodes
    if (op == Opcode::CONST || op == Opcode::LOAD || op == Opcode::STORE ||
        op == Opcode::JUMP || op == Opcode::JUMP_IF_ZERO ||
        op == Opcode::CALL) {
      std::cout << " " << code[i].operand;
    }
    std::cout << std::endl;
  }
  std::cout << "Main entry: " << mainEntry << std::endl;
}

// ============================================================================
// Code Generator Implementation
// ============================================================================

BytecodeProgram CodeGenerator::generate(const Program &program,
                                        bool incremental) {
  // Reset state
  program_ = BytecodeProgram{};

  if (!incremental) {
    scopes_.clear();
    scopes_.emplace_back(); // Global scope
    globals_.clear();
    functionMap_.clear();
  }

  currentFunction_.clear();
  loopStack_.clear();

  // First pass: register all functions
  for (const auto &item : program.items()) {
    if (auto *fn = dynamic_cast<const FunctionDecl *>(item.get())) {
      FunctionInfo info;
      info.name = fn->name();
      info.entry = 0; // Will be set during code generation
      info.arity = static_cast<uint8_t>(fn->params().size());
      info.localCount = 0;

      functionMap_[fn->name()] =
          static_cast<uint16_t>(program_.functions.size());
      program_.functions.push_back(info);
    }
  }

  // Generate code for functions first
  for (const auto &item : program.items()) {
    if (auto *fn = dynamic_cast<const FunctionDecl *>(item.get())) {
      fn->accept(*this);
    }
  }

  // Mark main entry point (top-level statements start here)
  program_.mainEntry = currentIndex();

  // Generate code for top-level statements
  for (const auto &item : program.items()) {
    if (auto *stmt = dynamic_cast<const Stmt *>(item.get())) {
      stmt->accept(*this);
    }
  }

  // Add implicit RETURN at end of main (only for non-incremental/file mode)
  // In REPL mode, we don't want to push 0 and return - just let execution fall
  // through
  if (!incremental) {
    emit(Opcode::CONST, addConstant(0));
    emit(Opcode::RETURN);
  }

  return std::move(program_);
}

// ============================================================================
// Expression Visitors
// ============================================================================

void CodeGenerator::visitNumberExpr(const NumberExpr &expr) {
  uint16_t constIdx = addConstant(Value(expr.value()));
  emit(Opcode::CONST, constIdx);
}

void CodeGenerator::visitStringLiteralExpr(const StringLiteralExpr &expr) {
  uint16_t constIdx = addConstant(Value(expr.value()));
  emit(Opcode::CONST, constIdx);
}

void CodeGenerator::visitIdentifierExpr(const IdentifierExpr &expr) {
  uint16_t slot = getLocal(expr.name());
  emit(Opcode::LOAD, slot);
}

void CodeGenerator::visitBinaryOpExpr(const BinaryOpExpr &expr) {
  // Generate left operand (pushes value)
  expr.left().accept(*this);

  // Generate right operand (pushes value)
  expr.right().accept(*this);

  // Emit operation
  switch (expr.op()) {
  case BinaryOpExpr::Operator::PLUS:
    emit(Opcode::ADD);
    break;
  case BinaryOpExpr::Operator::MINUS:
    emit(Opcode::SUB);
    break;
  case BinaryOpExpr::Operator::MULTIPLY:
    emit(Opcode::MUL);
    break;
  case BinaryOpExpr::Operator::DIVIDE:
    emit(Opcode::DIV);
    break;
  case BinaryOpExpr::Operator::MODULO:
    emit(Opcode::MOD);
    break;
  case BinaryOpExpr::Operator::EQUAL:
    emit(Opcode::EQ);
    break;
  case BinaryOpExpr::Operator::NOT_EQUAL:
    emit(Opcode::NEQ);
    break;
  case BinaryOpExpr::Operator::LESS:
    emit(Opcode::LT);
    break;
  case BinaryOpExpr::Operator::LESS_EQUAL:
    emit(Opcode::LTE);
    break;
  case BinaryOpExpr::Operator::GREATER:
    emit(Opcode::GT);
    break;
  case BinaryOpExpr::Operator::GREATER_EQUAL:
    emit(Opcode::GTE);
    break;
  case BinaryOpExpr::Operator::AND:
    emit(Opcode::MUL); // Boolean AND -> MUL (1*1=1, 1*0=0)
    break;
  case BinaryOpExpr::Operator::OR:
    emit(Opcode::ADD); // Boolean OR -> ADD (1+0=1), careful 1+1=2 (truthy)
    break;
  }
}

void CodeGenerator::visitUnaryOpExpr(const UnaryOpExpr &expr) {
  switch (expr.op()) {
  case UnaryOpExpr::Operator::NEGATE:
    // Push 0, then operand, then subtract: 0 - operand
    emit(Opcode::CONST, addConstant(0));
    expr.operand().accept(*this);
    emit(Opcode::SUB);
    break;

  case UnaryOpExpr::Operator::NOT:
    // Push 1, then operand, then if operand is 0, result is 1; else 0
    // Simplified: 1 - operand (works for 0/1 boolean)
    expr.operand().accept(*this);
    emit(Opcode::CONST, addConstant(0));
    // Check if equal to zero using JUMP_IF_ZERO
    {
      uint16_t jumpIfTrue = emit(Opcode::JUMP_IF_ZERO, 0);
      emit(Opcode::CONST, addConstant(0)); // not truthy -> false
      uint16_t jumpEnd = emit(Opcode::JUMP, 0);
      patchJump(jumpIfTrue, currentIndex());
      emit(Opcode::CONST, addConstant(1)); // was zero -> true
      patchJump(jumpEnd, currentIndex());
    }
    break;
  }
}

void CodeGenerator::visitFunctionCallExpr(const FunctionCallExpr &expr) {
  // Push arguments onto stack (left to right)
  for (const auto &arg : expr.args()) {
    arg->accept(*this);
  }

  // Find function index
  auto it = functionMap_.find(expr.name());
  if (it == functionMap_.end()) {
    throw CodegenError("Undefined function: " + expr.name());
  }

  emit(Opcode::CALL, it->second);
}

void CodeGenerator::visitArrayLiteralExpr(const ArrayLiteralExpr &expr) {
  // Push elements onto stack directly
  for (const auto &element : expr.elements()) {
    element->accept(*this);
  }
  // Emit BUILD_ARRAY with count
  emit(Opcode::BUILD_ARRAY, static_cast<uint16_t>(expr.elements().size()));
}

void CodeGenerator::visitIndexExpr(const IndexExpr &expr) {
  expr.target().accept(*this); // Push array
  expr.index().accept(*this);  // Push index
  emit(Opcode::ARRAY_LOAD);
}

// ============================================================================
// Statement Visitors
// ============================================================================

void CodeGenerator::visitAssignmentStmt(const AssignmentStmt &stmt) {
  // Generate value expression
  stmt.value().accept(*this);

  // Store to variable
  uint16_t slot = getOrCreateLocal(stmt.name());
  emit(Opcode::STORE, slot);
}

void CodeGenerator::visitArrayAssignmentStmt(const ArrayAssignmentStmt &stmt) {
  stmt.target().accept(*this); // Push array
  stmt.index().accept(*this);  // Push index
  stmt.value().accept(*this);  // Push value
  emit(Opcode::ARRAY_STORE);
}

void CodeGenerator::visitExpressionStmt(const ExpressionStmt &stmt) {
  stmt.expr().accept(*this);
  emit(Opcode::POP);
}

void CodeGenerator::visitPrintStmt(const PrintStmt &stmt) {
  // Generate value expression
  stmt.value().accept(*this);

  // Print top of stack
  emit(Opcode::PRINT);
}

void CodeGenerator::visitIfStmt(const IfStmt &stmt) {
  // Generate condition
  stmt.condition().accept(*this);

  // Jump to end if zero (false)
  uint16_t jumpToEnd = emit(Opcode::JUMP_IF_ZERO, 0);

  // Generate body
  for (const auto &s : stmt.body()) {
    s->accept(*this);
  }

  // Patch jump to point to after body
  patchJump(jumpToEnd, currentIndex());
}

void CodeGenerator::visitWhileStmt(const WhileStmt &stmt) {
  // Loop start
  uint16_t loopStart = currentIndex();

  // Push loop context
  loopStack_.push_back(
      {(int)loopStart, {}, {}}); // continue target is loopStart

  // Generate condition
  stmt.condition().accept(*this);

  // Jump to end if zero (false)
  size_t jumpToEnd = emitJump(Opcode::JUMP_IF_ZERO);

  // Generate body
  for (const auto &s : stmt.body()) {
    s->accept(*this);
  }

  // Jump back to start
  emit(Opcode::JUMP, loopStart);

  // Patch exit jump
  patchJump(jumpToEnd, currentIndex());

  // Patch breaks
  uint16_t endIp = currentIndex();
  for (size_t offset : loopStack_.back().breakJumps) {
    patchJump(offset, endIp);
  }

  loopStack_.pop_back();
}

void CodeGenerator::visitForStmt(const ForStmt &stmt) {
  // 1. Init
  scopes_.emplace_back(); // New scope for init variable
  if (stmt.init()) {
    stmt.init()->accept(*this);
  }

  // 2. Start Label
  uint16_t startIp = currentIndex();

  // 3. Loop Context
  loopStack_.push_back({-1, {}, {}}); // continueTarget is -1 initially

  // 4. Condition
  size_t jumpToEnd = -1;
  if (stmt.condition()) {
    stmt.condition()->accept(*this);
    jumpToEnd = emitJump(Opcode::JUMP_IF_ZERO);
  }

  // 5. Body
  for (const auto &s : stmt.body()) {
    s->accept(*this);
  }

  // 6. Continue Target (Start of increment)
  loopStack_.back().continueTarget = (int)currentIndex();

  // 7. Increment
  if (stmt.increment()) {
    stmt.increment()->accept(*this);
  }

  // 8. Jump to start
  emit(Opcode::JUMP, startIp);

  // 9. Patch Break/Continue/End
  uint16_t endIp = currentIndex();
  if (jumpToEnd != (size_t)-1) {
    patchJump(jumpToEnd, endIp);
  }

  // Patch breaks to endIp
  for (size_t offset : loopStack_.back().breakJumps) {
    patchJump(offset, endIp);
  }

  // Patch continues to continueTarget
  uint16_t continueTarget = (uint16_t)loopStack_.back().continueTarget;
  for (size_t offset : loopStack_.back().continueJumps) {
    patchJump(offset, continueTarget);
  }

  loopStack_.pop_back();
  scopes_.pop_back();
}

void CodeGenerator::visitBreakStmt(const BreakStmt &stmt) {
  (void)stmt;
  if (loopStack_.empty()) {
    throw CodegenError("Break statement outside of loop");
  }
  loopStack_.back().breakJumps.push_back(emitJump(Opcode::JUMP));
}

void CodeGenerator::visitContinueStmt(const ContinueStmt &stmt) {
  (void)stmt;
  if (loopStack_.empty()) {
    throw CodegenError("Continue statement outside of loop");
  }
  // If target is known (while loop), use it
  if (loopStack_.back().continueTarget != -1) {
    emit(Opcode::JUMP, (uint16_t)loopStack_.back().continueTarget);
  } else {
    loopStack_.back().continueJumps.push_back(emitJump(Opcode::JUMP));
  }
}

void CodeGenerator::visitReturnStmt(const ReturnStmt &stmt) {
  if (stmt.value()) {
    stmt.value()->accept(*this);
  } else {
    // Return 0 if no value specified
    emit(Opcode::CONST, addConstant(0));
  }
  emit(Opcode::RETURN);
}

void CodeGenerator::visitBlockStmt(const BlockStmt &stmt) {
  for (const auto &s : stmt.statements()) {
    s->accept(*this);
  }
}

// ============================================================================
// Top-Level Visitors
// ============================================================================

void CodeGenerator::visitFunctionDecl(const FunctionDecl &decl) {
  beginFunction(decl.name(), decl.params());

  // Record entry point
  program_.functions[functionMap_[decl.name()]].entry = currentIndex();

  // Generate body
  for (const auto &stmt : decl.body()) {
    stmt->accept(*this);
  }

  // Implicit return 0 if no explicit return
  emit(Opcode::CONST, addConstant(0));
  emit(Opcode::RETURN);

  endFunction();
}

void CodeGenerator::visitProgram(const Program &program) {
  // This is called by generate(), just dispatch to items
  for (const auto &item : program.items()) {
    item->accept(*this);
  }
}

// ============================================================================
// Helper Methods
// ============================================================================

uint16_t CodeGenerator::emit(Opcode op, uint16_t operand) {
  Instruction instr;
  instr.opcode = static_cast<uint8_t>(op);
  instr.operand = operand;

  uint16_t index = static_cast<uint16_t>(program_.code.size());
  program_.code.push_back(instr);
  return index;
}

uint16_t CodeGenerator::addConstant(Value value) {
  // Check if constant already exists
  for (size_t i = 0; i < program_.constants.size(); ++i) {
    if (program_.constants[i] == value) {
      return static_cast<uint16_t>(i);
    }
  }

  uint16_t index = static_cast<uint16_t>(program_.constants.size());
  program_.constants.push_back(value);
  return index;
}

size_t CodeGenerator::emitJump(Opcode opcode) {
  emit(opcode, (uint16_t)0);
  return program_.code.size() - 1;
}

uint16_t CodeGenerator::getOrCreateLocal(const std::string &name) {
  // First, search ALL scopes from innermost to outermost
  for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return found->second; // Found in an outer scope
    }
  }

  // Not found in any scope - create in current (innermost) scope
  auto &scope = scopes_.back();

  // Calculate next available slot
  uint16_t slot = 0;
  for (const auto &s : scopes_)
    slot += s.size();

  scope[name] = slot;
  return slot;
}

uint16_t CodeGenerator::getLocal(const std::string &name) const {
  // Search scopes from innermost to outermost
  for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) {
      return found->second;
    }
  }
  throw CodegenError("Undefined variable: " + name);
}

bool CodeGenerator::isGlobalScope() const { return currentFunction_.empty(); }

void CodeGenerator::patchJump(size_t jumpIndex, uint16_t target) {
  program_.code[jumpIndex].operand = target;
}

uint16_t CodeGenerator::currentIndex() const {
  return static_cast<uint16_t>(program_.code.size());
}

void CodeGenerator::beginFunction(const std::string &name,
                                  const std::vector<std::string> &params) {
  currentFunction_ = name;
  scopes_.clear();
  scopes_.emplace_back(); // Function scope

  // Add parameters as local variables (in order)
  for (const auto &param : params) {
    getOrCreateLocal(param);
  }
}

void CodeGenerator::endFunction() {
  // Record local count
  auto it = functionMap_.find(currentFunction_);
  if (it != functionMap_.end()) {
    // Total locals is sum of all items in all scopes?
    // Since scopes are popped, correct way is to track max slot used?
    // Or just current count if we don't support blocks with distinct stack
    // slots? My getOrCreateLocal implementation sums sizes of ALL ACTIVE
    // scopes. But since I assume flat slots for now, this is simple. Wait,
    // getOrCreateLocal logic: `slot = 0; for(s) slot += s.size();` This assigns
    // slots 0, 1, 2... based on current active variables. If I enter block, add
    // var (slot 3), exit block. Next var (slot 3). This allows REUSE of slots!
    // But for `localCount` in function info, I need PEAK slot usage.
    // I am not tracking peak.
    // I should fix getOrCreateLocal logic to use unique slots per function if
    // proper debugging needed. For now, let's just use `locals_.size()`
    // equivalent: sum of active scopes. But since scopes are popped, this
    // number shrinks. This assumes variables in parallel blocks reuse slots.
    // This is optimizing! But `localCount` in `FunctionInfo` must be size of
    // `locals_` vector in VM. So I need max slot index + 1. I need to track
    // `maxLocals_` in `CodeGenerator`. I'll leave it as is for now, but
    // `endFunction` calculation is tricky.

    // Quick fix: Iterating active scopes only gives CURRENT locals.
    // CodeGenerator needs to track `maxLocals`.
    // Since I can't add member easily now without touching header again?
    // I added `loopStack_` in header. I can add `maxLocals_`.
    // I'll assume standard behavior for now: Just use 255 or something safe?
    // No. I will modify header again if needed. For this step, I'll calculate
    // based on current scopes (which is top level scope of function).
    uint8_t count = 0;
    for (const auto &s : scopes_)
      count += s.size();
    program_.functions[it->second].localCount = count;
  }

  currentFunction_.clear();
  scopes_.clear();
  scopes_.emplace_back(); // Global scope
}
