#include "lexer.h"
#include <cctype>
#include <sstream>
#include <stdexcept>

// ============================================================================
// Token Methods
// ============================================================================

std::string Token::typeString() const {
  switch (type) {
  case TokenType::END_OF_FILE:
    return "EOF";
  case TokenType::ILLEGAL:
    return "ILLEGAL";
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::NUMBER:
    return "NUMBER";
  case TokenType::STRING:
    return "STRING";
  case TokenType::KW_LET:
    return "KW_LET";
  case TokenType::KW_FN:
    return "KW_FN";
  case TokenType::KW_IF:
    return "KW_IF";
  case TokenType::KW_ELSE:
    return "KW_ELSE";
  case TokenType::KW_WHILE:
    return "KW_WHILE";
  case TokenType::KW_RETURN:
    return "KW_RETURN";
  case TokenType::KW_PRINT:
    return "KW_PRINT";
  case TokenType::KW_FOR:
    return "KW_FOR";
  case TokenType::KW_BREAK:
    return "KW_BREAK";
  case TokenType::KW_CONTINUE:
    return "KW_CONTINUE";
  case TokenType::PLUS:
    return "PLUS";
  case TokenType::MINUS:
    return "MINUS";
  case TokenType::STAR:
    return "STAR";
  case TokenType::SLASH:
    return "SLASH";
  case TokenType::PERCENT:
    return "PERCENT";
  case TokenType::ASSIGN:
    return "ASSIGN";
  case TokenType::EQ:
    return "EQ";
  case TokenType::NEQ:
    return "NEQ";
  case TokenType::LT:
    return "LT";
  case TokenType::LTE:
    return "LTE";
  case TokenType::GT:
    return "GT";
  case TokenType::GTE:
    return "GTE";
  case TokenType::AND_AND:
    return "AND_AND";
  case TokenType::OR_OR:
    return "OR_OR";
  case TokenType::BANG:
    return "BANG";
  case TokenType::LPAREN:
    return "LPAREN";
  case TokenType::RPAREN:
    return "RPAREN";
  case TokenType::LBRACE:
    return "LBRACE";
  case TokenType::RBRACE:
    return "RBRACE";
  case TokenType::SEMICOLON:
    return "SEMICOLON";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::LBRACKET:
    return "LBRACKET";
  case TokenType::RBRACKET:
    return "RBRACKET";
  default:
    return "UNKNOWN";
  }
}

// ============================================================================
// Lexer Constructor
// ============================================================================

Lexer::Lexer(const std::string &source)
    : source_(source), index_(0), line_(1), column_(1) {}

// ============================================================================
// Core Character Methods
// ============================================================================

char Lexer::currentChar() const {
  if (isAtEnd()) {
    return '\0';
  }
  return source_[index_];
}

char Lexer::peekChar() const {
  if (index_ + 1 >= source_.length()) {
    return '\0';
  }
  return source_[index_ + 1];
}

bool Lexer::isAtEnd() const { return index_ >= source_.length(); }

void Lexer::advance() {
  if (!isAtEnd()) {
    if (source_[index_] == '\n') {
      line_++;
      column_ = 1;
    } else {
      column_++;
    }
    index_++;
  }
}

// ============================================================================
// Whitespace and Comment Handling
// ============================================================================

void Lexer::skipWhitespaceAndComments() {
  while (!isAtEnd()) {
    char c = currentChar();

    // Skip whitespace
    if (isWhitespace(c)) {
      advance();
      continue;
    }

    // Skip single-line comments
    if (c == '/' && peekChar() == '/') {
      // Skip until end of line
      while (!isAtEnd() && currentChar() != '\n') {
        advance();
      }
      // Skip the newline itself if present
      if (!isAtEnd() && currentChar() == '\n') {
        advance();
      }
      continue;
    }

    // Not whitespace or comment
    break;
  }
}

// ============================================================================
// Classification Methods
// ============================================================================

bool Lexer::isIdentifierStart(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Lexer::isIdentifierPart(char c) {
  return isIdentifierStart(c) || isDigit(c);
}

bool Lexer::isDigit(char c) { return c >= '0' && c <= '9'; }

bool Lexer::isWhitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

TokenType Lexer::keywordType(const std::string &ident) {
  if (ident == "let")
    return TokenType::KW_LET;
  if (ident == "fn")
    return TokenType::KW_FN;
  if (ident == "if")
    return TokenType::KW_IF;
  if (ident == "else")
    return TokenType::KW_ELSE;
  if (ident == "while")
    return TokenType::KW_WHILE;
  if (ident == "return")
    return TokenType::KW_RETURN;
  if (ident == "print")
    return TokenType::KW_PRINT;
  if (ident == "for")
    return TokenType::KW_FOR;
  if (ident == "break")
    return TokenType::KW_BREAK;
  if (ident == "continue")
    return TokenType::KW_CONTINUE;
  return TokenType::IDENTIFIER;
}

// ============================================================================
// Token Construction
// ============================================================================

Token Lexer::makeToken(TokenType type, const std::string &lexeme) const {
  return Token{type, lexeme, line_, column_};
}

// ============================================================================
// Lexing Methods
// ============================================================================

Token Lexer::lexNumber() {
  int startLine = line_;
  int startCol = column_;
  std::string lexeme;

  while (!isAtEnd() && isDigit(currentChar())) {
    lexeme += currentChar();
    advance();
  }

  return Token{TokenType::NUMBER, lexeme, startLine, startCol};
}

Token Lexer::lexString() {
  int startLine = line_;
  int startCol = column_;
  std::string lexeme;

  advance(); // Consume opening quote

  while (!isAtEnd() && currentChar() != '"') {
    // Handle escaped characters if needed, for now just basic text
    lexeme += currentChar();
    advance();
  }

  if (isAtEnd()) {
    throw LexerError("Unterminated string literal at line " +
                     std::to_string(startLine));
  }

  advance(); // Consume closing quote

  return Token{TokenType::STRING, lexeme, startLine, startCol};
}

Token Lexer::lexIdentifierOrKeyword() {
  int startLine = line_;
  int startCol = column_;
  std::string lexeme;

  while (!isAtEnd() && isIdentifierPart(currentChar())) {
    lexeme += currentChar();
    advance();
  }

  TokenType type = keywordType(lexeme);
  return Token{type, lexeme, startLine, startCol};
}

Token Lexer::lexOperatorOrDelimiter() {
  int startLine = line_;
  int startCol = column_;
  char c = currentChar();
  advance();

  switch (c) {
  case '+':
    return Token{TokenType::PLUS, "+", startLine, startCol};
  case '-':
    return Token{TokenType::MINUS, "-", startLine, startCol};
  case '*':
    return Token{TokenType::STAR, "*", startLine, startCol};
  case '%':
    return Token{TokenType::PERCENT, "%", startLine, startCol};
  case '(':
    return Token{TokenType::LPAREN, "(", startLine, startCol};
  case ')':
    return Token{TokenType::RPAREN, ")", startLine, startCol};
  case '{':
    return Token{TokenType::LBRACE, "{", startLine, startCol};
  case '}':
    return Token{TokenType::RBRACE, "}", startLine, startCol};
  case ';':
    return Token{TokenType::SEMICOLON, ";", startLine, startCol};
  case ',':
    return Token{TokenType::COMMA, ",", startLine, startCol};
  case '[':
    return Token{TokenType::LBRACKET, "[", startLine, startCol};
  case ']':
    return Token{TokenType::RBRACKET, "]", startLine, startCol};

  case '/':
    return Token{TokenType::SLASH, "/", startLine, startCol};

  case '=':
    if (currentChar() == '=') {
      advance();
      return Token{TokenType::EQ, "==", startLine, startCol};
    }
    return Token{TokenType::ASSIGN, "=", startLine, startCol};

  case '!':
    if (currentChar() == '=') {
      advance();
      return Token{TokenType::NEQ, "!=", startLine, startCol};
    }
    return Token{TokenType::BANG, "!", startLine, startCol};

  case '<':
    if (currentChar() == '=') {
      advance();
      return Token{TokenType::LTE, "<=", startLine, startCol};
    }
    return Token{TokenType::LT, "<", startLine, startCol};

  case '>':
    if (currentChar() == '=') {
      advance();
      return Token{TokenType::GTE, ">=", startLine, startCol};
    }
    return Token{TokenType::GT, ">", startLine, startCol};

  case '&':
    if (currentChar() == '&') {
      advance();
      return Token{TokenType::AND_AND, "&&", startLine, startCol};
    }
    // Single & is not supported
    {
      std::ostringstream oss;
      oss << "Unexpected character '&' at line " << startLine << ", column "
          << startCol << ". Use '&&' for logical AND.";
      throw LexerError(oss.str());
    }

  case '|':
    if (currentChar() == '|') {
      advance();
      return Token{TokenType::OR_OR, "||", startLine, startCol};
    }
    // Single | is not supported
    {
      std::ostringstream oss;
      oss << "Unexpected character '|' at line " << startLine << ", column "
          << startCol << ". Use '||' for logical OR.";
      throw LexerError(oss.str());
    }

  default:
    // Unsupported character
    {
      std::ostringstream oss;
      oss << "Illegal character '" << c << "' at line " << startLine
          << ", column " << startCol;
      throw LexerError(oss.str());
    }
  }
}

// ============================================================================
// Main Tokenize Method
// ============================================================================

std::vector<Token> Lexer::tokenize() {
  std::vector<Token> tokens;

  while (!isAtEnd()) {
    skipWhitespaceAndComments();

    if (isAtEnd()) {
      break;
    }

    char c = currentChar();

    if (isDigit(c)) {
      tokens.push_back(lexNumber());
    } else if (isIdentifierStart(c)) {
      tokens.push_back(lexIdentifierOrKeyword());
    } else if (c == '"') {
      tokens.push_back(lexString());
    } else {
      tokens.push_back(lexOperatorOrDelimiter());
    }
  }

  // Append end-of-file token
  tokens.push_back(Token{TokenType::END_OF_FILE, "", line_, column_});

  return tokens;
}
