#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <string>
#include <vector>
#include "common.h"

// ============================================================================
// Token Types for the Language
// ============================================================================

/**
 * Classification of all token types in the language
 */
enum class TokenType {
    // Special tokens
    END_OF_FILE = 0,
    ILLEGAL,

    // Identifiers and literals
    IDENTIFIER,
    NUMBER,

    // Keywords
    KW_LET,
    KW_FN,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_RETURN,
    KW_PRINT,

    // Arithmetic operators
    PLUS,       // +
    MINUS,      // -
    STAR,       // *
    SLASH,      // /
    PERCENT,    // %

    // Assignment and comparison
    ASSIGN,     // =
    EQ,         // ==
    NEQ,        // !=
    LT,         // <
    LTE,        // <=
    GT,         // >
    GTE,        // >=

    // Logical operators
    AND_AND,    // &&
    OR_OR,      // ||
    BANG,       // !

    // Delimiters and punctuation
    LPAREN,     // (
    RPAREN,     // )
    LBRACE,     // {
    RBRACE,     // }
    SEMICOLON,  // ;
    COMMA       // ,
};

// ============================================================================
// Token Structure
// ============================================================================

/**
 * Represents a single lexical token with type, text, and source location
 */
struct Token {
    TokenType type;
    std::string lexeme;  // Raw text from source
    int line;
    int column;

    /**
     * Helper to convert token type to string for debugging
     */
    std::string typeString() const;
};

// ============================================================================
// Lexer Class
// ============================================================================

/**
 * Lexical analyzer (tokenizer) for the compiler
 * Converts source code string into a sequence of tokens
 */
class Lexer {
public:
    /**
     * Initialize lexer with source code
     * @param source Source code string to tokenize
     */
    explicit Lexer(const std::string& source);

    /**
     * Tokenize entire source into a vector of tokens
     * @return Vector of tokens ending with END_OF_FILE
     * @throws LexerError on invalid characters or malformed tokens
     */
    std::vector<Token> tokenize();

private:
    const std::string source_;
    size_t index_;
    int line_;
    int column_;

    // ========================================================================
    // Core Lexing Methods
    // ========================================================================

    /**
     * Get current character without consuming it
     * @return Current char or '\0' if at end
     */
    char currentChar() const;

    /**
     * Look ahead one character without consuming
     * @return Next char or '\0' if at end
     */
    char peekChar() const;

    /**
     * Check if lexer is at end of input
     * @return True if index_ >= source_.length()
     */
    bool isAtEnd() const;

    /**
     * Consume current character and advance state
     * Updates index_, line_, and column_
     */
    void advance();

    /**
     * Skip whitespace and comments
     * Handles spaces, tabs, newlines, and // single-line comments
     */
    void skipWhitespaceAndComments();

    // ========================================================================
    // Token Construction Methods
    // ========================================================================

    /**
     * Create a token at current position
     * @param type The token type
     * @param lexeme The raw text of the token
     * @return Constructed Token with current line/column
     */
    Token makeToken(TokenType type, const std::string& lexeme) const;

    /**
     * Lex a numeric literal (decimal integers only)
     * @return Token of type NUMBER
     */
    Token lexNumber();

    /**
     * Lex an identifier or keyword
     * @return Token of type IDENTIFIER or KW_*
     */
    Token lexIdentifierOrKeyword();

    /**
     * Lex an operator or delimiter
     * @return Token of appropriate operator/delimiter type
     * @throws LexerError for unsupported character combinations
     */
    Token lexOperatorOrDelimiter();

    // ========================================================================
    // Helper Classification Methods
    // ========================================================================

    /**
     * Check if character can start an identifier
     * @param c Character to check
     * @return True if c is [a-zA-Z_]
     */
    static bool isIdentifierStart(char c);

    /**
     * Check if character can continue an identifier
     * @param c Character to check
     * @return True if c is [a-zA-Z0-9_]
     */
    static bool isIdentifierPart(char c);

    /**
     * Check if character is a decimal digit
     * @param c Character to check
     * @return True if c is [0-9]
     */
    static bool isDigit(char c);

    /**
     * Check if character is whitespace
     * @param c Character to check
     * @return True if c is space, tab, newline, or carriage return
     */
    static bool isWhitespace(char c);

    /**
     * Determine if string is a keyword
     * @param ident Identifier string
     * @return Corresponding KW_* type or IDENTIFIER if not a keyword
     */
    static TokenType keywordType(const std::string& ident);
};

#endif // COMPILER_LEXER_H
