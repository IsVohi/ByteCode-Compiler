#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "ast.h"
#include "lexer.h"
#include <memory>
#include <string>
#include <vector>

/**
 * Parser class for syntax analysis.
 * Consumes a stream of tokens from the Lexer and produces an Abstract Syntax
 * Tree (AST).
 *
 * Parser class for syntax analysis.
 * Consumes a stream of tokens from the Lexer and produces an Abstract Syntax
 * Tree (AST).
 */
class Parser {
public:
  /**
   * Construct a parser from a token stream.
   *
   * @param tokens Vector of tokens produced by the Lexer
   */
  explicit Parser(const std::vector<Token> &tokens);

  // ========================================================================
  // Main Entry Points
  // ========================================================================

  /**
   * Parse the entire program.
   * Entry point for parsing - processes all statements and function
   * declarations.
   *
   * @return Unique pointer to the root Program node
   * @throws ParserError if syntax is invalid
   */
  std::unique_ptr<Program> parseProgram();

  /**
   * Parse a single function declaration.
   *
   * @return Unique pointer to a FunctionDecl node
   * @throws ParserError if function syntax is invalid
   */
  std::unique_ptr<FunctionDecl> parseFunction();

  /**
   * Parse a block of statements enclosed in braces.
   *
   * @return Vector of parsed statements
   * @throws ParserError if block syntax is invalid
   */
  std::vector<std::unique_ptr<Stmt>> parseBlock();

  /**
   * Parse a single statement.
   * Dispatches to specialized parsers based on token type.
   *
   * @return Unique pointer to a Stmt node
   * @throws ParserError if statement syntax is invalid
   */
  std::unique_ptr<Stmt> parseStatement();

  // ========================================================================
  // Expression Parsing
  // ========================================================================

  /**
   * Parse an expression with full precedence handling.
   * Handles logical OR as lowest precedence.
   *
   * @return Unique pointer to an Expr node
   * @throws ParserError if expression syntax is invalid
   */
  std::unique_ptr<Expr> parseExpression();

  /**
   * Parse logical OR expression: left || right
   *
   * @return Unique pointer to an Expr node
   */
  std::unique_ptr<Expr> parseLogicalOr();

  /**
   * Parse logical AND expression: left && right
   *
   * @return Unique pointer to an Expr node
   */
  std::unique_ptr<Expr> parseLogicalAnd();

  /**
   * Parse comparison expression: left == right, left != right, etc.
   *
   * @return Unique pointer to an Expr node
   */
  std::unique_ptr<Expr> parseComparison();

  /**
   * Parse relational expression: left < right, left <= right, etc.
   *
   * @return Unique pointer to an Expr node
   */
  std::unique_ptr<Expr> parseRelational();

  /**
   * Parse additive expression: left + right, left - right
   *
   * @return Unique pointer to an Expr node
   */
  std::unique_ptr<Expr> parseTerm();

  /**
   * Parse multiplicative expression: left * right, left / right, left % right
   *
   * @return Unique pointer to an Expr node
   */
  std::unique_ptr<Expr> parseFactor();

  /**
   * Parse unary expression: -expr, !expr
   *
   * @return Unique pointer to an Expr node
   */
  std::unique_ptr<Expr> parseUnary();

  /**
   * Parse primary expression: number, identifier, function call, or
   * parenthesized expression. Lowest level of expression hierarchy.
   *
   * @return Unique pointer to an Expr node
   */
  std::unique_ptr<Expr> parsePrimary();

private:
  // ========================================================================
  // Statement Parsing Helpers
  // ========================================================================

  std::unique_ptr<Stmt> parseVarDecl();
  std::unique_ptr<Stmt> parseAssignment();
  std::unique_ptr<Expr> parsePostfix(std::unique_ptr<Expr> left);
  std::unique_ptr<Expr> parseArrayLiteral();
  std::unique_ptr<Stmt> parseIfStatement();
  std::unique_ptr<Stmt> parseWhileStatement();
  std::unique_ptr<Stmt> parseForStatement();
  std::unique_ptr<Stmt> parseBreakStatement();
  std::unique_ptr<Stmt> parseContinueStatement();
  std::unique_ptr<Stmt> parseReturnStatement();
  std::unique_ptr<Stmt> parsePrintStatement();

  // ========================================================================
  // Token Navigation
  // ========================================================================

  const std::vector<Token> &tokens_; ///< Reference to token stream
  size_t current_;                   ///< Current position in token stream

  /**
   * Get the current token without consuming it.
   *
   * @return Reference to the current token
   */
  const Token &currentToken() const;

  /**
   * Look ahead one token without consuming it.
   *
   * @return Reference to the next token, or EOF token if at end
   */
  const Token &peekToken() const;

  /**
   * Look ahead N tokens without consuming.
   *
   * @param n Number of tokens to look ahead
   * @return Reference to the token at current + n, or EOF if beyond end
   */
  const Token &peek(size_t n) const;

  /**
   * Check if we've reached the end of tokens.
   *
   * @return True if current token is END_OF_FILE
   */
  bool isAtEnd() const;

  /**
   * Consume the current token and move to the next one.
   */
  void advance();

  /**
   * Check if current token matches a specific type.
   *
   * @param type The TokenType to check
   * @return True if current token has the specified type
   */
  bool check(TokenType type) const;

  /**
   * Check if current token matches any of the given types.
   *
   * @param types Vector of TokenTypes to check
   * @return True if current token matches any type in the vector
   */
  bool checkAny(const std::vector<TokenType> &types) const;

  /**
   * Consume a token of a specific type, or throw an error.
   *
   * @param type The expected TokenType
   * @param message Error message if type doesn't match
   * @throws ParserError if current token doesn't match type
   */
  void expect(TokenType type, const std::string &message);

  /**
   * Match and consume tokens of specific types.
   *
   * @param types Vector of TokenTypes to match
   * @return True and advances if current token matches any type, false
   * otherwise
   */
  bool match(const std::vector<TokenType> &types);

  // ========================================================================
  // Error Handling
  // ========================================================================

  /**
   * Report a parsing error at current position.
   *
   * @param message Descriptive error message
   * @throws ParserError
   */
  void error(const std::string &message);

  /**
   * Report an error with expected vs. found token types.
   *
   * @param expected The expected TokenType
   * @param found The actual TokenType encountered
   * @throws ParserError
   */
  void errorAt(TokenType expected, TokenType found);

  /**
   * Report an error with expected type and actual token content.
   *
   * @param expected Descriptive string of what was expected
   * @throws ParserError
   */
  void errorExpected(const std::string &expected);

  /**
   * Helper to generate error messages with line/column info from tokens.
   *
   * @param message Base error message
   * @return Formatted error message with location info
   */
  std::string formatError(const std::string &message) const;
};

#endif // COMPILER_PARSER_H
