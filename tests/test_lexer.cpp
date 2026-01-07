#include <gtest/gtest.h>
#include "lexer.h"

// ============================================================================
// Test Fixture
// ============================================================================

class LexerTest : public ::testing::Test {
protected:
    /**
     * Helper to extract token types from a source string
     */
    std::vector<TokenType> tokenizeTypes(const std::string& source) {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        std::vector<TokenType> types;
        for (const auto& token : tokens) {
            types.push_back(token.type);
        }
        return types;
    }

    /**
     * Helper to extract token lexemes from a source string
     */
    std::vector<std::string> tokenizeLexemes(const std::string& source) {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        std::vector<std::string> lexemes;
        for (const auto& token : tokens) {
            lexemes.push_back(token.lexeme);
        }
        return lexemes;
    }
};

// ============================================================================
// NUMBER TOKENS
// ============================================================================

TEST_F(LexerTest, TokenizeZero) {
    auto types = tokenizeTypes("0");
    EXPECT_EQ(types[0], TokenType::NUMBER);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeSimpleNumber) {
    auto types = tokenizeTypes("42");
    EXPECT_EQ(types[0], TokenType::NUMBER);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeMultiDigitNumber) {
    auto types = tokenizeTypes("12345");
    EXPECT_EQ(types[0], TokenType::NUMBER);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, NumberLexeme) {
    auto lexemes = tokenizeLexemes("999");
    EXPECT_EQ(lexemes[0], "999");
}

// ============================================================================
// IDENTIFIER TOKENS
// ============================================================================

TEST_F(LexerTest, TokenizeSimpleIdentifier) {
    auto types = tokenizeTypes("x");
    EXPECT_EQ(types[0], TokenType::IDENTIFIER);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeMultiCharIdentifier) {
    auto types = tokenizeTypes("sum");
    EXPECT_EQ(types[0], TokenType::IDENTIFIER);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeIdentifierWithUnderscore) {
    auto types = tokenizeTypes("_hidden");
    EXPECT_EQ(types[0], TokenType::IDENTIFIER);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeIdentifierWithDigits) {
    auto types = tokenizeTypes("factorial1");
    EXPECT_EQ(types[0], TokenType::IDENTIFIER);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, IdentifierLexeme) {
    auto lexemes = tokenizeLexemes("myVar");
    EXPECT_EQ(lexemes[0], "myVar");
}

// ============================================================================
// KEYWORD TOKENS
// ============================================================================

TEST_F(LexerTest, TokenizeLetKeyword) {
    auto types = tokenizeTypes("let");
    EXPECT_EQ(types[0], TokenType::KW_LET);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeFnKeyword) {
    auto types = tokenizeTypes("fn");
    EXPECT_EQ(types[0], TokenType::KW_FN);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeIfKeyword) {
    auto types = tokenizeTypes("if");
    EXPECT_EQ(types[0], TokenType::KW_IF);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeElseKeyword) {
    auto types = tokenizeTypes("else");
    EXPECT_EQ(types[0], TokenType::KW_ELSE);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeWhileKeyword) {
    auto types = tokenizeTypes("while");
    EXPECT_EQ(types[0], TokenType::KW_WHILE);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeReturnKeyword) {
    auto types = tokenizeTypes("return");
    EXPECT_EQ(types[0], TokenType::KW_RETURN);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizePrintKeyword) {
    auto types = tokenizeTypes("print");
    EXPECT_EQ(types[0], TokenType::KW_PRINT);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

// ============================================================================
// SINGLE-CHARACTER OPERATORS
// ============================================================================

TEST_F(LexerTest, TokenizePlus) {
    auto types = tokenizeTypes("+");
    EXPECT_EQ(types[0], TokenType::PLUS);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeMinus) {
    auto types = tokenizeTypes("-");
    EXPECT_EQ(types[0], TokenType::MINUS);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeMultiply) {
    auto types = tokenizeTypes("*");
    EXPECT_EQ(types[0], TokenType::STAR);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeDivide) {
    auto types = tokenizeTypes("/");
    EXPECT_EQ(types[0], TokenType::SLASH);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeModulo) {
    auto types = tokenizeTypes("%");
    EXPECT_EQ(types[0], TokenType::PERCENT);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeLess) {
    auto types = tokenizeTypes("<");
    EXPECT_EQ(types[0], TokenType::LT);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeGreater) {
    auto types = tokenizeTypes(">");
    EXPECT_EQ(types[0], TokenType::GT);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeBang) {
    auto types = tokenizeTypes("!");
    EXPECT_EQ(types[0], TokenType::BANG);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

// ============================================================================
// MULTI-CHARACTER OPERATORS
// ============================================================================

TEST_F(LexerTest, TokenizeAssign) {
    auto types = tokenizeTypes("=");
    EXPECT_EQ(types[0], TokenType::ASSIGN);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeEquality) {
    auto types = tokenizeTypes("==");
    EXPECT_EQ(types[0], TokenType::EQ);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeNotEqual) {
    auto types = tokenizeTypes("!=");
    EXPECT_EQ(types[0], TokenType::NEQ);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeLessEqual) {
    auto types = tokenizeTypes("<=");
    EXPECT_EQ(types[0], TokenType::LTE);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeGreaterEqual) {
    auto types = tokenizeTypes(">=");
    EXPECT_EQ(types[0], TokenType::GTE);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeLogicalAnd) {
    auto types = tokenizeTypes("&&");
    EXPECT_EQ(types[0], TokenType::AND_AND);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeLogicalOr) {
    auto types = tokenizeTypes("||");
    EXPECT_EQ(types[0], TokenType::OR_OR);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

// ============================================================================
// DELIMITERS AND PUNCTUATION
// ============================================================================

TEST_F(LexerTest, TokenizeLeftParen) {
    auto types = tokenizeTypes("(");
    EXPECT_EQ(types[0], TokenType::LPAREN);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeRightParen) {
    auto types = tokenizeTypes(")");
    EXPECT_EQ(types[0], TokenType::RPAREN);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeLeftBrace) {
    auto types = tokenizeTypes("{");
    EXPECT_EQ(types[0], TokenType::LBRACE);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeRightBrace) {
    auto types = tokenizeTypes("}");
    EXPECT_EQ(types[0], TokenType::RBRACE);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeSemicolon) {
    auto types = tokenizeTypes(";");
    EXPECT_EQ(types[0], TokenType::SEMICOLON);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, TokenizeComma) {
    auto types = tokenizeTypes(",");
    EXPECT_EQ(types[0], TokenType::COMMA);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

// ============================================================================
// WHITESPACE HANDLING
// ============================================================================

TEST_F(LexerTest, SkipSpaces) {
    auto types = tokenizeTypes("  let  ");
    EXPECT_EQ(types[0], TokenType::KW_LET);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, SkipTabs) {
    auto types = tokenizeTypes("\tlet\t");
    EXPECT_EQ(types[0], TokenType::KW_LET);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, SkipNewlines) {
    auto types = tokenizeTypes("let\nx");
    EXPECT_EQ(types[0], TokenType::KW_LET);
    EXPECT_EQ(types[1], TokenType::IDENTIFIER);
    EXPECT_EQ(types[2], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, SkipMultipleWhitespace) {
    auto types = tokenizeTypes("  \t\n  x  \t\n  ");
    EXPECT_EQ(types[0], TokenType::IDENTIFIER);
    EXPECT_EQ(types[1], TokenType::END_OF_FILE);
}

// ============================================================================
// COMMENT HANDLING
// ============================================================================

TEST_F(LexerTest, SkipSingleLineComment) {
    auto types = tokenizeTypes("let // comment\nx");
    EXPECT_EQ(types[0], TokenType::KW_LET);
    EXPECT_EQ(types[1], TokenType::IDENTIFIER);
    EXPECT_EQ(types[2], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, SkipCommentAtEnd) {
    auto types = tokenizeTypes("let x; // end of statement");
    EXPECT_EQ(types[0], TokenType::KW_LET);
    EXPECT_EQ(types[1], TokenType::IDENTIFIER);
    EXPECT_EQ(types[2], TokenType::SEMICOLON);
    EXPECT_EQ(types[3], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, SkipMultipleComments) {
    auto types = tokenizeTypes("let // comment 1\nx // comment 2\n");
    EXPECT_EQ(types[0], TokenType::KW_LET);
    EXPECT_EQ(types[1], TokenType::IDENTIFIER);
    EXPECT_EQ(types[2], TokenType::END_OF_FILE);
}

// ============================================================================
// COMPLEX PROGRAMS
// ============================================================================

TEST_F(LexerTest, SimpleLet) {
    auto types = tokenizeTypes("let x = 42;");
    EXPECT_EQ(types.size(), static_cast<size_t>(6));
    EXPECT_EQ(types[0], TokenType::KW_LET);
    EXPECT_EQ(types[1], TokenType::IDENTIFIER);
    EXPECT_EQ(types[2], TokenType::ASSIGN);
    EXPECT_EQ(types[3], TokenType::NUMBER);
    EXPECT_EQ(types[4], TokenType::SEMICOLON);
    EXPECT_EQ(types[5], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, FunctionDeclaration) {
    std::string src = "fn add(a, b) { return a + b; }";
    auto types = tokenizeTypes(src);

    std::vector<TokenType> expected = {
        TokenType::KW_FN,
        TokenType::IDENTIFIER,      // add
        TokenType::LPAREN,
        TokenType::IDENTIFIER,      // a
        TokenType::COMMA,
        TokenType::IDENTIFIER,      // b
        TokenType::RPAREN,
        TokenType::LBRACE,
        TokenType::KW_RETURN,
        TokenType::IDENTIFIER,      // a
        TokenType::PLUS,
        TokenType::IDENTIFIER,      // b
        TokenType::SEMICOLON,
        TokenType::RBRACE,
        TokenType::END_OF_FILE
    };

    EXPECT_EQ(types, expected);
}

TEST_F(LexerTest, AllArithmeticOps) {
    auto types = tokenizeTypes("a + b - c * d / e % f");
    EXPECT_EQ(types[0], TokenType::IDENTIFIER);
    EXPECT_EQ(types[1], TokenType::PLUS);
    EXPECT_EQ(types[2], TokenType::IDENTIFIER);
    EXPECT_EQ(types[3], TokenType::MINUS);
    EXPECT_EQ(types[4], TokenType::IDENTIFIER);
    EXPECT_EQ(types[5], TokenType::STAR);
    EXPECT_EQ(types[6], TokenType::IDENTIFIER);
    EXPECT_EQ(types[7], TokenType::SLASH);
    EXPECT_EQ(types[8], TokenType::IDENTIFIER);
    EXPECT_EQ(types[9], TokenType::PERCENT);
    EXPECT_EQ(types[10], TokenType::IDENTIFIER);
    EXPECT_EQ(types[11], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, ComparisonOps) {
    auto types = tokenizeTypes("x < y <= z > w >= v == u != t");
    EXPECT_EQ(types[1], TokenType::LT);
    EXPECT_EQ(types[3], TokenType::LTE);
    EXPECT_EQ(types[5], TokenType::GT);
    EXPECT_EQ(types[7], TokenType::GTE);
    EXPECT_EQ(types[9], TokenType::EQ);
    EXPECT_EQ(types[11], TokenType::NEQ);
}

TEST_F(LexerTest, LogicalOps) {
    auto types = tokenizeTypes("a && b || !c");
    EXPECT_EQ(types[1], TokenType::AND_AND);
    EXPECT_EQ(types[3], TokenType::OR_OR);
    EXPECT_EQ(types[4], TokenType::BANG);
}

TEST_F(LexerTest, IfStatement) {
    std::string src = "if (x > 10) { print(x); }";
    auto types = tokenizeTypes(src);

    EXPECT_EQ(types[0], TokenType::KW_IF);
    EXPECT_EQ(types[1], TokenType::LPAREN);
    EXPECT_EQ(types[2], TokenType::IDENTIFIER);
    EXPECT_EQ(types[3], TokenType::GT);
    EXPECT_EQ(types[4], TokenType::NUMBER);
    EXPECT_EQ(types[5], TokenType::RPAREN);
    EXPECT_EQ(types[6], TokenType::LBRACE);
    EXPECT_EQ(types[7], TokenType::KW_PRINT);
    EXPECT_EQ(types[8], TokenType::LPAREN);
    EXPECT_EQ(types[9], TokenType::IDENTIFIER);
    EXPECT_EQ(types[10], TokenType::RPAREN);
    EXPECT_EQ(types[11], TokenType::SEMICOLON);
    EXPECT_EQ(types[12], TokenType::RBRACE);
    EXPECT_EQ(types[13], TokenType::END_OF_FILE);
}

// ============================================================================
// LEXEME VERIFICATION
// ============================================================================

TEST_F(LexerTest, CorrectNumberLexemes) {
    auto lexemes = tokenizeLexemes("0 42 12345");
    EXPECT_EQ(lexemes[0], "0");
    EXPECT_EQ(lexemes[1], "42");
    EXPECT_EQ(lexemes[2], "12345");
}

TEST_F(LexerTest, CorrectIdentifierLexemes) {
    auto lexemes = tokenizeLexemes("x sum _hidden");
    EXPECT_EQ(lexemes[0], "x");
    EXPECT_EQ(lexemes[1], "sum");
    EXPECT_EQ(lexemes[2], "_hidden");
}

TEST_F(LexerTest, CorrectKeywordLexemes) {
    auto lexemes = tokenizeLexemes("let fn if");
    EXPECT_EQ(lexemes[0], "let");
    EXPECT_EQ(lexemes[1], "fn");
    EXPECT_EQ(lexemes[2], "if");
}

TEST_F(LexerTest, CorrectOperatorLexemes) {
    auto lexemes = tokenizeLexemes("+ - * / % = == !=");
    EXPECT_EQ(lexemes[0], "+");
    EXPECT_EQ(lexemes[1], "-");
    EXPECT_EQ(lexemes[2], "*");
    EXPECT_EQ(lexemes[3], "/");
    EXPECT_EQ(lexemes[4], "%");
    EXPECT_EQ(lexemes[5], "=");
    EXPECT_EQ(lexemes[6], "==");
    EXPECT_EQ(lexemes[7], "!=");
}

// ============================================================================
// ERROR HANDLING
// ============================================================================

TEST_F(LexerTest, IllegalCharacterThrows) {
    EXPECT_THROW(tokenizeTypes("@"), LexerError);
}

TEST_F(LexerTest, SingleAmpersandThrows) {
    EXPECT_THROW(tokenizeTypes("&"), LexerError);
}

TEST_F(LexerTest, SinglePipeThrows) {
    EXPECT_THROW(tokenizeTypes("|"), LexerError);
}

TEST_F(LexerTest, MultipleIllegalChars) {
    EXPECT_THROW(tokenizeTypes("x # y"), LexerError);
}

// ============================================================================
// LINE AND COLUMN TRACKING
// ============================================================================

TEST_F(LexerTest, LineTracking) {
    Lexer lexer("let\nx\ny");
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[0].line, 1);  // let
    EXPECT_EQ(tokens[1].line, 2);  // x
    EXPECT_EQ(tokens[2].line, 3);  // y
}

TEST_F(LexerTest, ColumnTracking) {
    Lexer lexer("let x = 42");
    auto tokens = lexer.tokenize();

    EXPECT_EQ(tokens[0].column, 1);  // let
    EXPECT_EQ(tokens[1].column, 5);  // x
    EXPECT_EQ(tokens[2].column, 7);  // =
    EXPECT_EQ(tokens[3].column, 9);  // 42
}

// ============================================================================
// EDGE CASES
// ============================================================================

TEST_F(LexerTest, EmptySource) {
    auto types = tokenizeTypes("");
    EXPECT_EQ(types.size(), static_cast<size_t>(1));
    EXPECT_EQ(types[0], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, OnlyWhitespace) {
    auto types = tokenizeTypes("   \t\n  ");
    EXPECT_EQ(types.size(), static_cast<size_t>(1));
    EXPECT_EQ(types[0], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, OnlyComment) {
    auto types = tokenizeTypes("// this is a comment");
    EXPECT_EQ(types.size(), static_cast<size_t>(1));
    EXPECT_EQ(types[0], TokenType::END_OF_FILE);
}

TEST_F(LexerTest, LongIdentifier) {
    auto lexemes = tokenizeLexemes("_very_long_identifier_name_with_many_parts");
    EXPECT_EQ(lexemes[0], "_very_long_identifier_name_with_many_parts");
}

TEST_F(LexerTest, LongNumber) {
    auto lexemes = tokenizeLexemes("99999999999999");
    EXPECT_EQ(lexemes[0], "99999999999999");
}

TEST_F(LexerTest, CompleteProgram) {
    std::string program = R"(
        fn fibonacci(n) {
            if (n <= 1) {
                return n;
            }
            let a = 0;
            let b = 1;
            while (n > 2) {
                let temp = a + b;
                a = b;
                b = temp;
                n = n - 1;
            }
            return b;
        }

        let result = fibonacci(10);
        print(result);
    )";

    Lexer lexer(program);
    auto tokens = lexer.tokenize();

    // Should have at least: fn, identifier, (, identifier, ), {, if, ...
    EXPECT_GT(tokens.size(), static_cast<size_t>(30));
    EXPECT_EQ(tokens.back().type, TokenType::END_OF_FILE);

    // Verify no unexpected ILLEGAL tokens
    for (const auto& token : tokens) {
        EXPECT_NE(token.type, TokenType::ILLEGAL);
    }
}
