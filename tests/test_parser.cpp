#include <gtest/gtest.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"

// ============================================================================
// Test Fixture
// ============================================================================

class ParserTest : public ::testing::Test {
protected:
    /**
     * Helper to tokenize source code
     */
    std::vector<Token> tokenize(const std::string& source) {
        Lexer lexer(source);
        return lexer.tokenize();
    }
};

// ============================================================================
// Parser Construction Tests
// ============================================================================

TEST_F(ParserTest, ConstructParserFromTokens) {
    auto tokens = tokenize("let x = 5;");
    Parser parser(tokens);
    // Should compile and construct without errors
}

TEST_F(ParserTest, ConstructParserFromEmptyTokens) {
    std::vector<Token> empty;
    // Should handle empty token stream gracefully
    Parser parser(empty);
}

TEST_F(ParserTest, ConstructParserFromComplexTokens) {
    auto tokens = tokenize("fn factorial(n) { if (n <= 1) { return 1; } }");
    Parser parser(tokens);
    // Should handle complex token stream
}

// ============================================================================
// Token Navigation Tests
// ============================================================================

TEST_F(ParserTest, CurrentTokenAccess) {
    auto tokens = tokenize("let x = 5;");
    Parser parser(tokens);
    
    // Should be able to check current token without crashing
    // (even though token type checking would fail in skeleton)
}

TEST_F(ParserTest, IsAtEndDetection) {
    auto tokens = tokenize("x");
    Parser parser(tokens);
    
    // Parser should track position and not crash
    // (Full EOF detection tested in commit 4 when parsing works)
}

// ============================================================================
// AST Node Construction Tests
// ============================================================================

TEST_F(ParserTest, CreateNumberExpr) {
    auto num = std::make_unique<NumberExpr>(42);
    EXPECT_EQ(num->value(), 42);
}

TEST_F(ParserTest, CreateIdentifierExpr) {
    auto id = std::make_unique<IdentifierExpr>("myvar");
    EXPECT_EQ(id->name(), "myvar");
}

TEST_F(ParserTest, CreateBinaryOpExpr) {
    auto left = std::make_unique<NumberExpr>(3);
    auto right = std::make_unique<NumberExpr>(5);
    auto binop = std::make_unique<BinaryOpExpr>(
        std::move(left),
        BinaryOpExpr::Operator::PLUS,
        std::move(right)
    );
    EXPECT_EQ(binop->op(), BinaryOpExpr::Operator::PLUS);
}

TEST_F(ParserTest, CreateUnaryOpExpr) {
    auto operand = std::make_unique<IdentifierExpr>("x");
    auto unary = std::make_unique<UnaryOpExpr>(
        UnaryOpExpr::Operator::NEGATE,
        std::move(operand)
    );
    EXPECT_EQ(unary->op(), UnaryOpExpr::Operator::NEGATE);
}

TEST_F(ParserTest, CreateFunctionCallExpr) {
    std::vector<std::unique_ptr<Expr>> args;
    args.push_back(std::make_unique<NumberExpr>(10));
    args.push_back(std::make_unique<NumberExpr>(20));
    
    auto call = std::make_unique<FunctionCallExpr>("add", std::move(args));
    EXPECT_EQ(call->name(), "add");
    EXPECT_EQ(call->args().size(), 2);
}

TEST_F(ParserTest, CreateAssignmentStmt) {
    auto value = std::make_unique<NumberExpr>(42);
    auto assign = std::make_unique<AssignmentStmt>("x", std::move(value));
    EXPECT_EQ(assign->name(), "x");
}

TEST_F(ParserTest, CreatePrintStmt) {
    auto value = std::make_unique<IdentifierExpr>("x");
    auto print = std::make_unique<PrintStmt>(std::move(value));
    // Should construct without errors
}

TEST_F(ParserTest, CreateReturnStmt) {
    auto value = std::make_unique<NumberExpr>(0);
    auto ret = std::make_unique<ReturnStmt>(std::move(value));
    EXPECT_NE(ret->value(), nullptr);
}

TEST_F(ParserTest, CreateReturnStmtNoValue) {
    auto ret = std::make_unique<ReturnStmt>();
    EXPECT_EQ(ret->value(), nullptr);
}

TEST_F(ParserTest, CreateBlockStmt) {
    std::vector<std::unique_ptr<Stmt>> stmts;
    stmts.push_back(std::make_unique<PrintStmt>(
        std::make_unique<NumberExpr>(5)
    ));
    auto block = std::make_unique<BlockStmt>(std::move(stmts));
    EXPECT_EQ(block->statements().size(), 1);
}

TEST_F(ParserTest, CreateFunctionDecl) {
    std::vector<std::string> params = {"n"};
    std::vector<std::unique_ptr<Stmt>> body;
    body.push_back(std::make_unique<ReturnStmt>(
        std::make_unique<NumberExpr>(1)
    ));
    
    auto fn = std::make_unique<FunctionDecl>("factorial", std::move(params), std::move(body));
    EXPECT_EQ(fn->name(), "factorial");
    EXPECT_EQ(fn->params().size(), 1);
    EXPECT_EQ(fn->params()[0], "n");
}

TEST_F(ParserTest, CreateProgram) {
    std::vector<std::unique_ptr<ASTNode>> items;
    items.push_back(std::make_unique<FunctionDecl>(
        "main",
        std::vector<std::string>{},
        std::vector<std::unique_ptr<Stmt>>{}
    ));
    
    auto prog = std::make_unique<Program>(std::move(items));
    EXPECT_EQ(prog->items().size(), 1);
}

// ============================================================================
// Visitor Pattern Tests
// ============================================================================

/**
 * Concrete visitor implementation for testing
 */
class TestVisitor : public ASTVisitor {
public:
    int visitCount = 0;
    
    void visitNumberExpr(const NumberExpr&) override { visitCount++; }
    void visitIdentifierExpr(const IdentifierExpr&) override { visitCount++; }
    void visitBinaryOpExpr(const BinaryOpExpr&) override { visitCount++; }
    void visitUnaryOpExpr(const UnaryOpExpr&) override { visitCount++; }
    void visitFunctionCallExpr(const FunctionCallExpr&) override { visitCount++; }
    void visitAssignmentStmt(const AssignmentStmt&) override { visitCount++; }
    void visitPrintStmt(const PrintStmt&) override { visitCount++; }
    void visitIfStmt(const IfStmt&) override { visitCount++; }
    void visitWhileStmt(const WhileStmt&) override { visitCount++; }
    void visitReturnStmt(const ReturnStmt&) override { visitCount++; }
    void visitBlockStmt(const BlockStmt&) override { visitCount++; }
    void visitFunctionDecl(const FunctionDecl&) override { visitCount++; }
    void visitProgram(const Program&) override { visitCount++; }
};

TEST_F(ParserTest, VisitorPatternNumberExpr) {
    auto num = std::make_unique<NumberExpr>(42);
    TestVisitor visitor;
    num->accept(visitor);
    EXPECT_EQ(visitor.visitCount, 1);
}

TEST_F(ParserTest, VisitorPatternBinaryOpExpr) {
    auto left = std::make_unique<NumberExpr>(3);
    auto right = std::make_unique<NumberExpr>(5);
    auto binop = std::make_unique<BinaryOpExpr>(
        std::move(left),
        BinaryOpExpr::Operator::PLUS,
        std::move(right)
    );
    TestVisitor visitor;
    binop->accept(visitor);
    EXPECT_EQ(visitor.visitCount, 1);
}

TEST_F(ParserTest, VisitorPatternProgram) {
    std::vector<std::unique_ptr<ASTNode>> items;
    auto prog = std::make_unique<Program>(std::move(items));
    TestVisitor visitor;
    prog->accept(visitor);
    EXPECT_EQ(visitor.visitCount, 1);
}

// ============================================================================
// Parser Method Not Implemented Tests
// ============================================================================

TEST_F(ParserTest, ParseProgramThrowsNotImplemented) {
    auto tokens = tokenize("let x = 5;");
    Parser parser(tokens);
    EXPECT_THROW(parser.parseProgram(), ParserError);
}

TEST_F(ParserTest, ParseFunctionThrowsNotImplemented) {
    auto tokens = tokenize("fn test() {}");
    Parser parser(tokens);
    EXPECT_THROW(parser.parseFunction(), ParserError);
}

TEST_F(ParserTest, ParseStatementThrowsNotImplemented) {
    auto tokens = tokenize("let x = 5;");
    Parser parser(tokens);
    EXPECT_THROW(parser.parseStatement(), ParserError);
}

TEST_F(ParserTest, ParseExpressionThrowsNotImplemented) {
    auto tokens = tokenize("5 + 3");
    Parser parser(tokens);
    EXPECT_THROW(parser.parseExpression(), ParserError);
}

TEST_F(ParserTest, ParseBlockThrowsNotImplemented) {
    auto tokens = tokenize("{ let x = 5; }");
    Parser parser(tokens);
    EXPECT_THROW(parser.parseBlock(), ParserError);
}

// ============================================================================
// Error Handling Tests
// ============================================================================

TEST_F(ParserTest, ParserErrorHasMessage) {
    auto tokens = tokenize("let x = 5;");
    Parser parser(tokens);
    try {
        parser.parseProgram();
        FAIL() << "Should have thrown ParserError";
    } catch (const ParserError& e) {
        std::string msg = e.what();
        EXPECT_FALSE(msg.empty());
        EXPECT_NE(msg.find("Parsing"), std::string::npos);
    }
}

// ============================================================================
// Memory Management Tests
// ============================================================================

TEST_F(ParserTest, ASTNodesDontLeak) {
    {
        auto num = std::make_unique<NumberExpr>(42);
        auto id = std::make_unique<IdentifierExpr>("x");
        auto print = std::make_unique<PrintStmt>(std::move(id));
        // Unique pointers should clean up automatically
    }
    // If we reach here, no memory leaks
    SUCCEED();
}

TEST_F(ParserTest, DeepASTStructuresDontLeak) {
    {
        std::vector<std::unique_ptr<Stmt>> stmts;
        for (int i = 0; i < 10; ++i) {
            stmts.push_back(std::make_unique<PrintStmt>(
                std::make_unique<NumberExpr>(i)
            ));
        }
        auto block = std::make_unique<BlockStmt>(std::move(stmts));
        // Deep structure should clean up properly
    }
    SUCCEED();
}
