#include "parser.h"
#include "common.h"
#include <sstream>
#include <string>

// ============================================================================
// Constructor
// ============================================================================

Parser::Parser(const std::vector<Token>& tokens)
    : tokens_(tokens), current_(0) {}

// ============================================================================
// Main Entry Points - all throw "not implemented" for now
// ============================================================================

std::unique_ptr<Program> Parser::parseProgram() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<FunctionDecl> Parser::parseFunction() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::vector<std::unique_ptr<Stmt>> Parser::parseBlock() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

// ============================================================================
// Expression Parsing - all throw "not implemented" for now
// ============================================================================

std::unique_ptr<Expr> Parser::parseExpression() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Expr> Parser::parseLogicalOr() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Expr> Parser::parseLogicalAnd() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Expr> Parser::parseComparison() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Expr> Parser::parseRelational() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Expr> Parser::parseTerm() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Expr> Parser::parseFactor() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Expr> Parser::parseUnary() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    throw ParserError(std::string("Parsing not yet implemented - coming in next commit"));
}

// ============================================================================
// Token Navigation
// ============================================================================

const Token& Parser::currentToken() const {
    if (current_ < tokens_.size()) {
        return tokens_[current_];
    }
    static const Token eofToken{TokenType::END_OF_FILE, "", 0, 0};
    return eofToken;
}

const Token& Parser::peekToken() const {
    return peek(1);
}

const Token& Parser::peek(size_t n) const {
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
        return false;
    }
    return currentToken().type == type;
}

bool Parser::checkAny(const std::vector<TokenType>& types) const {
    for (TokenType t : types) {
        if (check(t)) {
            return true;
        }
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& message) {
    (void)message; // suppress unused for now; real logic will use this later
    if (!check(type)) {
        errorAt(type, currentToken().type);
    }
    advance();
}

bool Parser::match(const std::vector<TokenType>& types) {
    if (checkAny(types)) {
        advance();
        return true;
    }
    return false;
}

// ============================================================================
// Error Handling
// ============================================================================

void Parser::error(const std::string& message) {
    throw ParserError(formatError(message));
}

void Parser::errorAt(TokenType expected, TokenType found) {
    std::ostringstream oss;
    oss << "Expected token type " << static_cast<int>(expected)
        << " but found type " << static_cast<int>(found)
        << " at line " << currentToken().line
        << ", column " << currentToken().column;
    throw ParserError(oss.str());
}

void Parser::errorExpected(const std::string& expected) {
    std::ostringstream oss;
    oss << "Expected " << expected
        << " but found '" << currentToken().lexeme << "'"
        << " at line " << currentToken().line
        << ", column " << currentToken().column;
    throw ParserError(oss.str());
}

std::string Parser::formatError(const std::string& message) const {
    std::ostringstream oss;
    oss << message
        << " (line " << currentToken().line
        << ", column " << currentToken().column << ")";
    return oss.str();
}
