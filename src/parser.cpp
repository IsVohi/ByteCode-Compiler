#include "parser.h"
#include "common.h"
#include <sstream>
#include <string>

// ============================================================================
// Constructor
// ============================================================================

Parser::Parser(const std::vector<Token> &tokens)
    : tokens_(tokens), current_(0) {}

// ============================================================================
// Main Entry Points
// ============================================================================

std::unique_ptr<Program> Parser::parseProgram() {
  std::vector<std::unique_ptr<ASTNode>> items;

  while (!isAtEnd()) {
    if (check(TokenType::KW_FN)) {
      items.push_back(parseFunction());
    } else {
      items.push_back(parseStatement());
    }
  }

  return std::make_unique<Program>(std::move(items));
}

std::unique_ptr<FunctionDecl> Parser::parseFunction() {
  expect(TokenType::KW_FN, "Expected 'fn' keyword");

  if (!check(TokenType::IDENTIFIER)) {
    errorExpected("function name");
  }
  std::string name = currentToken().lexeme;
  advance();

  expect(TokenType::LPAREN, "Expected '(' after function name");

  // Parse parameter list
  std::vector<std::string> params;
  if (!check(TokenType::RPAREN)) {
    do {
      if (!check(TokenType::IDENTIFIER)) {
        errorExpected("parameter name");
      }
      params.push_back(currentToken().lexeme);
      advance();
    } while (match({TokenType::COMMA}));
  }

  expect(TokenType::RPAREN, "Expected ')' after parameters");
  expect(TokenType::LBRACE, "Expected '{' before function body");

  // Parse function body
  std::vector<std::unique_ptr<Stmt>> body;
  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    body.push_back(parseStatement());
  }

  expect(TokenType::RBRACE, "Expected '}' after function body");

  return std::make_unique<FunctionDecl>(std::move(name), std::move(params),
                                        std::move(body));
}

std::vector<std::unique_ptr<Stmt>> Parser::parseBlock() {
  expect(TokenType::LBRACE, "Expected '{' to start block");

  std::vector<std::unique_ptr<Stmt>> statements;
  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    statements.push_back(parseStatement());
  }

  expect(TokenType::RBRACE, "Expected '}' after block");

  return statements;
}

std::unique_ptr<Stmt> Parser::parseStatement() {
  if (check(TokenType::KW_LET)) {
    return parseVarDecl();
  }
  if (check(TokenType::KW_IF)) {
    return parseIfStatement();
  }
  if (check(TokenType::KW_WHILE)) {
    return parseWhileStatement();
  }
  if (check(TokenType::KW_FOR)) {
    return parseForStatement();
  }
  if (check(TokenType::KW_BREAK)) {
    return parseBreakStatement();
  }
  if (check(TokenType::KW_CONTINUE)) {
    return parseContinueStatement();
  }
  if (check(TokenType::KW_RETURN)) {
    return parseReturnStatement();
  }
  if (check(TokenType::KW_PRINT)) {
    return parsePrintStatement();
  }
  if (check(TokenType::LBRACE)) {
    return std::make_unique<BlockStmt>(parseBlock());
  }

  // Expression statement or Assignment
  auto expr = parseExpression();

  if (check(TokenType::ASSIGN)) {
    advance(); // consume '='
    auto value = parseExpression();
    expect(TokenType::SEMICOLON, "Expected ';' after assignment");

    if (auto *idExpr = dynamic_cast<IdentifierExpr *>(expr.get())) {
      return std::make_unique<AssignmentStmt>(idExpr->name(), std::move(value));
    } else if (auto *idxExpr = dynamic_cast<IndexExpr *>(expr.get())) {
      return std::make_unique<ArrayAssignmentStmt>(
          idxExpr->takeTarget(), idxExpr->takeIndex(), std::move(value));
    } else {
      error("Invalid assignment target");
    }
  }

  expect(TokenType::SEMICOLON, "Expected ';' after expression");
  return std::make_unique<ExpressionStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::parseVarDecl() {
  expect(TokenType::KW_LET, "Expected 'let' keyword");

  if (!check(TokenType::IDENTIFIER)) {
    errorExpected("variable name");
  }
  std::string name = currentToken().lexeme;
  advance();

  expect(TokenType::ASSIGN, "Expected '=' after variable name");

  auto value = parseExpression();

  expect(TokenType::SEMICOLON, "Expected ';' after variable declaration");

  return std::make_unique<AssignmentStmt>(std::move(name), std::move(value));
}

std::unique_ptr<Stmt> Parser::parseAssignment() {
  std::string name = currentToken().lexeme;
  advance(); // consume identifier

  expect(TokenType::ASSIGN, "Expected '=' in assignment");

  auto value = parseExpression();

  expect(TokenType::SEMICOLON, "Expected ';' after assignment");

  return std::make_unique<AssignmentStmt>(std::move(name), std::move(value));
}

// Replaced by generic assignment handling in parseStatement, keeping this for
// now but unused in new logic? Actually I'll remove parseAssignment definition
// here since I replaced call site in parseStatement (in logic above) BUT I
// haven't replaced it in the content yet. Let's implement parseArrayLiteral and
// parsePostfix at the end of file or appropriate place.

std::unique_ptr<Expr> Parser::parseArrayLiteral() {
  expect(TokenType::LBRACKET, "Expected '['");
  std::vector<std::unique_ptr<Expr>> elements;
  if (!check(TokenType::RBRACKET)) {
    do {
      elements.push_back(parseExpression());
    } while (match({TokenType::COMMA}));
  }
  expect(TokenType::RBRACKET, "Expected ']'");
  return std::make_unique<ArrayLiteralExpr>(std::move(elements));
}

std::unique_ptr<Expr> Parser::parsePostfix(std::unique_ptr<Expr> left) {
  while (check(TokenType::LBRACKET)) {
    advance(); // consume '['
    auto index = parseExpression();
    expect(TokenType::RBRACKET, "Expected ']' after index");
    left = std::make_unique<IndexExpr>(std::move(left), std::move(index));
  }
  return left;
}

std::unique_ptr<Stmt> Parser::parseIfStatement() {
  expect(TokenType::KW_IF, "Expected 'if' keyword");
  expect(TokenType::LPAREN, "Expected '(' after 'if'");

  auto condition = parseExpression();

  expect(TokenType::RPAREN, "Expected ')' after if condition");
  expect(TokenType::LBRACE, "Expected '{' after if condition");

  std::vector<std::unique_ptr<Stmt>> body;
  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    body.push_back(parseStatement());
  }

  expect(TokenType::RBRACE, "Expected '}' after if body");

  // Note: IfStmt doesn't have else branch in current AST
  // We could add it later if needed

  return std::make_unique<IfStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseWhileStatement() {
  expect(TokenType::KW_WHILE, "Expected 'while' keyword");
  expect(TokenType::LPAREN, "Expected '(' after 'while'");

  auto condition = parseExpression();

  expect(TokenType::RPAREN, "Expected ')' after while condition");
  expect(TokenType::LBRACE, "Expected '{' after while condition");

  std::vector<std::unique_ptr<Stmt>> body;
  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    body.push_back(parseStatement());
  }

  expect(TokenType::RBRACE, "Expected '}' after while body");

  return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseForStatement() {
  expect(TokenType::KW_FOR, "Expected 'for' keyword");
  expect(TokenType::LPAREN, "Expected '(' after 'for'");

  // Init
  std::unique_ptr<Stmt> init = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    if (check(TokenType::KW_LET)) {
      init = parseVarDecl();
    } else if (check(TokenType::IDENTIFIER) &&
               peek(1).type == TokenType::ASSIGN) {
      init = parseAssignment();
    } else {
      auto expr = parseExpression();
      expect(TokenType::SEMICOLON, "Expected ';' after expression in for init");
      init = std::make_unique<PrintStmt>(std::move(expr));
    }
  } else {
    advance(); // consume semicolon
  }

  // Condition
  std::unique_ptr<Expr> cond = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    cond = parseExpression();
  }
  expect(TokenType::SEMICOLON, "Expected ';' after for condition");

  // Increment
  std::unique_ptr<Stmt> inc = nullptr;
  if (!check(TokenType::RPAREN)) {
    if (check(TokenType::IDENTIFIER) && peek(1).type == TokenType::ASSIGN) {
      // Parse assignment without consuming semicolon
      std::string name = currentToken().lexeme;
      advance();
      expect(TokenType::ASSIGN, "Expected '='");
      auto value = parseExpression();
      inc = std::make_unique<AssignmentStmt>(std::move(name), std::move(value));
    } else {
      auto expr = parseExpression();
      inc = std::make_unique<PrintStmt>(std::move(expr));
    }
  }
  expect(TokenType::RPAREN, "Expected ')' after for clauses");

  expect(TokenType::LBRACE, "Expected '{' to start for body");
  std::vector<std::unique_ptr<Stmt>> body;
  while (!check(TokenType::RBRACE) && !isAtEnd()) {
    body.push_back(parseStatement());
  }
  expect(TokenType::RBRACE, "Expected '}' after for body");

  return std::make_unique<ForStmt>(std::move(init), std::move(cond),
                                   std::move(inc), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseBreakStatement() {
  expect(TokenType::KW_BREAK, "Expected 'break' keyword");
  expect(TokenType::SEMICOLON, "Expected ';'");
  return std::make_unique<BreakStmt>();
}

std::unique_ptr<Stmt> Parser::parseContinueStatement() {
  expect(TokenType::KW_CONTINUE, "Expected 'continue' keyword");
  expect(TokenType::SEMICOLON, "Expected ';'");
  return std::make_unique<ContinueStmt>();
}

std::unique_ptr<Stmt> Parser::parseReturnStatement() {
  expect(TokenType::KW_RETURN, "Expected 'return' keyword");

  std::unique_ptr<Expr> value = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    value = parseExpression();
  }

  expect(TokenType::SEMICOLON, "Expected ';' after return statement");

  return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::parsePrintStatement() {
  expect(TokenType::KW_PRINT, "Expected 'print' keyword");
  expect(TokenType::LPAREN, "Expected '(' after 'print'");

  auto value = parseExpression();

  expect(TokenType::RPAREN, "Expected ')' after print argument");
  expect(TokenType::SEMICOLON, "Expected ';' after print statement");

  return std::make_unique<PrintStmt>(std::move(value));
}

// ============================================================================
// Expression Parsing - Precedence Climbing
// ============================================================================

std::unique_ptr<Expr> Parser::parseExpression() { return parseLogicalOr(); }

std::unique_ptr<Expr> Parser::parseLogicalOr() {
  auto left = parseLogicalAnd();

  while (check(TokenType::OR_OR)) {
    advance();
    auto right = parseLogicalAnd();
    left = std::make_unique<BinaryOpExpr>(
        std::move(left), BinaryOpExpr::Operator::OR, std::move(right));
  }

  return left;
}

std::unique_ptr<Expr> Parser::parseLogicalAnd() {
  auto left = parseComparison();

  while (check(TokenType::AND_AND)) {
    advance();
    auto right = parseComparison();
    left = std::make_unique<BinaryOpExpr>(
        std::move(left), BinaryOpExpr::Operator::AND, std::move(right));
  }

  return left;
}

std::unique_ptr<Expr> Parser::parseComparison() {
  auto left = parseRelational();

  while (check(TokenType::EQ) || check(TokenType::NEQ)) {
    BinaryOpExpr::Operator op = check(TokenType::EQ)
                                    ? BinaryOpExpr::Operator::EQUAL
                                    : BinaryOpExpr::Operator::NOT_EQUAL;
    advance();
    auto right = parseRelational();
    left =
        std::make_unique<BinaryOpExpr>(std::move(left), op, std::move(right));
  }

  return left;
}

std::unique_ptr<Expr> Parser::parseRelational() {
  auto left = parseTerm();

  while (check(TokenType::LT) || check(TokenType::LTE) ||
         check(TokenType::GT) || check(TokenType::GTE)) {
    BinaryOpExpr::Operator op;
    if (check(TokenType::LT))
      op = BinaryOpExpr::Operator::LESS;
    else if (check(TokenType::LTE))
      op = BinaryOpExpr::Operator::LESS_EQUAL;
    else if (check(TokenType::GT))
      op = BinaryOpExpr::Operator::GREATER;
    else
      op = BinaryOpExpr::Operator::GREATER_EQUAL;

    advance();
    auto right = parseTerm();
    left =
        std::make_unique<BinaryOpExpr>(std::move(left), op, std::move(right));
  }

  return left;
}

std::unique_ptr<Expr> Parser::parseTerm() {
  auto left = parseFactor();

  while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
    BinaryOpExpr::Operator op = check(TokenType::PLUS)
                                    ? BinaryOpExpr::Operator::PLUS
                                    : BinaryOpExpr::Operator::MINUS;
    advance();
    auto right = parseFactor();
    left =
        std::make_unique<BinaryOpExpr>(std::move(left), op, std::move(right));
  }

  return left;
}

std::unique_ptr<Expr> Parser::parseFactor() {
  auto left = parseUnary();

  while (check(TokenType::STAR) || check(TokenType::SLASH) ||
         check(TokenType::PERCENT)) {
    BinaryOpExpr::Operator op;
    if (check(TokenType::STAR))
      op = BinaryOpExpr::Operator::MULTIPLY;
    else if (check(TokenType::SLASH))
      op = BinaryOpExpr::Operator::DIVIDE;
    else
      op = BinaryOpExpr::Operator::MODULO;

    advance();
    auto right = parseUnary();
    left =
        std::make_unique<BinaryOpExpr>(std::move(left), op, std::move(right));
  }

  return left;
}

std::unique_ptr<Expr> Parser::parseUnary() {
  if (check(TokenType::MINUS)) {
    advance();
    auto operand = parseUnary();
    return std::make_unique<UnaryOpExpr>(UnaryOpExpr::Operator::NEGATE,
                                         std::move(operand));
  }
  if (check(TokenType::BANG)) {
    advance();
    auto operand = parseUnary();
    return std::make_unique<UnaryOpExpr>(UnaryOpExpr::Operator::NOT,
                                         std::move(operand));
  }

  return parsePostfix(parsePrimary());
}

std::unique_ptr<Expr> Parser::parsePrimary() {
  // Array literal
  if (check(TokenType::LBRACKET)) {
    return parseArrayLiteral();
  }

  // Number literal
  if (check(TokenType::NUMBER)) {
    int value = std::stoi(currentToken().lexeme);
    advance();
    return std::make_unique<NumberExpr>(value);
  }

  // String literal
  if (check(TokenType::STRING)) {
    std::string value = currentToken().lexeme;
    advance();
    return std::make_unique<StringLiteralExpr>(std::move(value));
  }

  // Identifier or function call
  if (check(TokenType::IDENTIFIER)) {
    std::string name = currentToken().lexeme;
    advance();

    // Check for function call
    if (check(TokenType::LPAREN)) {
      advance(); // consume '('

      std::vector<std::unique_ptr<Expr>> args;
      if (!check(TokenType::RPAREN)) {
        do {
          args.push_back(parseExpression());
        } while (match({TokenType::COMMA}));
      }

      expect(TokenType::RPAREN, "Expected ')' after function arguments");
      return std::make_unique<FunctionCallExpr>(std::move(name),
                                                std::move(args));
    }

    return std::make_unique<IdentifierExpr>(std::move(name));
  }

  // Parenthesized expression
  if (check(TokenType::LPAREN)) {
    advance();
    auto expr = parseExpression();
    expect(TokenType::RPAREN, "Expected ')' after expression");
    return expr;
  }

  errorExpected("expression");
  return nullptr; // unreachable
}

// ============================================================================
// Token Navigation
// ============================================================================

const Token &Parser::currentToken() const {
  if (current_ < tokens_.size()) {
    return tokens_[current_];
  }
  static const Token eofToken{TokenType::END_OF_FILE, "", 0, 0};
  return eofToken;
}

const Token &Parser::peekToken() const { return peek(1); }

const Token &Parser::peek(size_t n) const {
  if (current_ + n < tokens_.size()) {
    return tokens_[current_ + n];
  }
  static const Token eofToken{TokenType::END_OF_FILE, "", 0, 0};
  return eofToken;
}

bool Parser::isAtEnd() const {
  return currentToken().type == TokenType::END_OF_FILE;
}

void Parser::advance() {
  if (!isAtEnd()) {
    ++current_;
  }
}

bool Parser::check(TokenType type) const {
  if (isAtEnd()) {
    return type == TokenType::END_OF_FILE;
  }
  return currentToken().type == type;
}

bool Parser::checkAny(const std::vector<TokenType> &types) const {
  for (TokenType t : types) {
    if (check(t)) {
      return true;
    }
  }
  return false;
}

void Parser::expect(TokenType type, const std::string &message) {
  if (!check(type)) {
    error(message);
  }
  advance();
}

bool Parser::match(const std::vector<TokenType> &types) {
  if (checkAny(types)) {
    advance();
    return true;
  }
  return false;
}

// ============================================================================
// Error Handling
// ============================================================================

void Parser::error(const std::string &message) {
  throw ParserError(formatError(message));
}

void Parser::errorAt(TokenType expected, TokenType found) {
  std::ostringstream oss;
  oss << "Expected token type " << static_cast<int>(expected)
      << " but found type " << static_cast<int>(found) << " at line "
      << currentToken().line << ", column " << currentToken().column;
  throw ParserError(oss.str());
}

void Parser::errorExpected(const std::string &expected) {
  std::ostringstream oss;
  oss << "Expected " << expected << " but found '" << currentToken().lexeme
      << "'"
      << " at line " << currentToken().line << ", column "
      << currentToken().column;
  throw ParserError(oss.str());
}

std::string Parser::formatError(const std::string &message) const {
  std::ostringstream oss;
  oss << message << " (line " << currentToken().line << ", column "
      << currentToken().column << ")";
  return oss.str();
}
