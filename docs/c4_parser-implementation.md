# How I Did Commit 4 - Full Parser Implementation

## What Was the Goal of Commit 4?

The goal was to **actually implement the parsing logic**. Commit 3 set up the AST structure and parser skeleton - now we fill in all those "not implemented" methods with real code.

Think of it like:
1. **Commit 3**: Built the blueprint (AST nodes + parser interface)
2. **Commit 4**: Actually built the house (parsing logic)

## What is Recursive Descent Parsing?

We use a technique called **recursive descent parsing**. It works like this:

```
Code: 3 + 5 * 2

How the parser thinks:
1. "I see 3... that's a number"
2. "I see +... that means addition"
3. "But wait! * has higher precedence than +"
4. "So 5 * 2 should be evaluated first"
5. "Result: 3 + (5 * 2) = 13"
```

Each operator level has its own parse function, arranged by precedence.

## What Did I Implement?

### 1. **parseProgram()** - The Entry Point

```cpp
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
```

This reads the entire program - functions and statements mixed together.

### 2. **Precedence Climbing** - Expression Parsing

Operators are parsed in order of precedence (lowest to highest):

```
Level 1: parseLogicalOr()      ||
Level 2: parseLogicalAnd()     &&
Level 3: parseEquality()       == !=
Level 4: parseComparison()     < <= > >=
Level 5: parseTerm()           + -
Level 6: parseFactor()         * / %
Level 7: parseUnary()          - !
Level 8: parsePrimary()        numbers, identifiers, calls, parens
```

**Example: Parsing `a + b * c`**

```
parseTerm() called
├── parseFactor() → returns IdentifierExpr("a")
├── sees "+" operator
├── parseFactor() again for right side
│   ├── parseUnary() → parsePrimary() → IdentifierExpr("b")
│   ├── sees "*" operator
│   ├── parsePrimary() → IdentifierExpr("c")
│   └── returns BinaryOpExpr(b * c)
└── returns BinaryOpExpr(a + BinaryOpExpr(b * c))
```

Result: `a + (b * c)` - multiplication happens first!

### 3. **Statement Parsing**

Each statement type has its own method:

```cpp
parseVarDecl()      // let x = 42;
parseIfStatement()  // if (cond) { body }
parseWhileStatement()// while (cond) { body }
parseForStatement() // for (init; cond; incr) { body }
parseReturnStatement()// return expr;
parsePrintStatement()// print(expr);
parseAssignment()   // x = 42;
```

### 4. **Function Parsing**

```cpp
std::unique_ptr<FunctionDecl> Parser::parseFunction() {
    expect(TokenType::KW_FN);               // "fn"
    std::string name = currentToken().lexeme;
    advance();                               // function name
    expect(TokenType::LPAREN);              // "("
    auto params = parseParameterList();     // a, b, c
    expect(TokenType::RPAREN);              // ")"
    expect(TokenType::LBRACE);              // "{"
    auto body = parseBlockBody();           // statements
    expect(TokenType::RBRACE);              // "}"
    
    return std::make_unique<FunctionDecl>(name, params, std::move(body));
}
```

## Key Parsing Techniques

### 1. **Lookahead with check() and match()**

```cpp
if (check(TokenType::KW_IF)) {
    return parseIfStatement();
}
```

We peek at the next token without consuming it.

### 2. **Consuming with expect()**

```cpp
expect(TokenType::SEMICOLON, "Expected ';' after statement");
```

If the token isn't what we expect, we throw an error with a helpful message.

### 3. **Error Recovery**

When we hit an error, we include line and column information:

```cpp
Parser error: Expected ')' but found ';' at line 5, column 12
```

## Files Modified

| File | Changes |
|------|---------|
| src/parser.cpp | 400+ lines of parsing logic |
| tests/test_parser.cpp | 30+ new parsing tests |

## Test Examples

### Simple Expression Parsing
```cpp
TEST(ParserTest, ParseBinaryExpression) {
    auto tokens = tokenize("3 + 5");
    Parser parser(tokens);
    auto expr = parser.parseExpression();
    
    auto* binary = dynamic_cast<BinaryOpExpr*>(expr.get());
    EXPECT_EQ(binary->op(), Operator::PLUS);
}
```

### Complex Statement Parsing
```cpp
TEST(ParserTest, ParseWhileLoop) {
    auto tokens = tokenize("while (x > 0) { x = x - 1; }");
    Parser parser(tokens);
    auto stmt = parser.parseStatement();
    
    auto* whileStmt = dynamic_cast<WhileStmt*>(stmt.get());
    EXPECT_NE(whileStmt, nullptr);
}
```

## What Works Now

Before Commit 4:
```cpp
parser.parseExpression();  // Throws "not implemented"
```

After Commit 4:
```cpp
parser.parseProgram();     // Returns complete AST!
```

You can now parse entire programs like:
```javascript
fn add(a, b) {
    return a + b;
}

let x = 10;
let y = 20;
print(add(x, y));  // 30
```

## Summary

Commit 4 turns the parser skeleton into a working parser. It:
- ✅ Implements all parsing methods
- ✅ Handles operator precedence correctly
- ✅ Parses functions, statements, and expressions
- ✅ Provides helpful error messages
- ✅ Returns proper AST trees
- ✅ Passes 30+ new tests

The parser is now ready to feed its AST to the Code Generator in Commit 5!

---

**Key Takeaway:** Commit 4 is the heart of the front-end. It takes tokens and builds a tree structure that represents your program.
