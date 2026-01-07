'use client';

import Link from 'next/link';
import { useState } from 'react';

const DOCS_SECTIONS = [
    {
        title: 'Getting Started',
        items: [
            {
                id: 'overview', title: 'Overview', content: `# ByteCode Compiler

A modern, optimizing bytecode compiler built from scratch in C++.

## Key Features

- **Complete Pipeline**: Lexer → Parser → AST → Optimizer → CodeGen → VM
- **Optimizations**: Constant folding, dead code elimination
- **Data Types**: Integers, strings, arrays
- **Control Flow**: if/else, while, for loops, break, continue
- **Functions**: First-class functions with recursion
- **Interactive REPL**: Real-time code execution

## Quick Start

\`\`\`javascript
fn factorial(n) {
    if (n <= 1) { return 1; }
    return n * factorial(n - 1);
}

print(factorial(5));  // Output: 120
\`\`\`` },
            {
                id: 'syntax', title: 'Syntax Guide', content: `# Syntax Reference

## Variables

\`\`\`javascript
let x = 10;
let name = "hello";
let arr = [1, 2, 3];
\`\`\`

## Functions

\`\`\`javascript
fn add(a, b) {
    return a + b;
}

let result = add(3, 5);  // 8
\`\`\`

## Control Flow

\`\`\`javascript
// If-Else
if (x > 0) {
    print("positive");
} else {
    print("non-positive");
}

// For Loop
for (let i = 0; i < 10; i = i + 1) {
    print(i);
}

// While Loop
while (x > 0) {
    x = x - 1;
}
\`\`\`

## Arrays

\`\`\`javascript
let arr = [1, 2, 3, 4, 5];
print(arr[0]);     // Access: 1
arr[2] = 99;       // Modify
print(arr);        // [1, 2, 99, 4, 5]
\`\`\`

## Operators

| Type | Operators |
|------|-----------|
| Arithmetic | + - * / % |
| Comparison | == != < > <= >= |
| Logical | && \|\| ! |` },
        ]
    },
    {
        title: 'Architecture',
        items: [
            {
                id: 'pipeline', title: 'Compiler Pipeline', content: `# Compiler Pipeline

\`\`\`
Source → Lexer → Parser → AST → Optimizer → CodeGen → Bytecode → VM → Output
\`\`\`

## 1. Lexer

Tokenizes source code into meaningful tokens:
- Keywords: fn, let, if, else, while, for, return, print
- Operators: +, -, *, /, ==, !=, <, >, etc.
- Literals: numbers, strings
- Identifiers: variable and function names

## 2. Parser

Recursive descent parser that builds an Abstract Syntax Tree (AST).

**Expression Precedence** (lowest to highest):
1. Logical OR (\|\|)
2. Logical AND (&&)
3. Equality (==, !=)
4. Comparison (<, <=, >, >=)
5. Addition (+, -)
6. Multiplication (*, /, %)
7. Unary (-, !)
8. Primary (literals, identifiers, calls)

## 3. Optimizer

Three optimization passes:
- **Constant Folding**: Evaluate constant expressions at compile time
- **Dead Code Elimination**: Remove unreachable code
- **Function Inlining**: Inline small functions

## 4. Code Generator

Translates AST into stack-based bytecode instructions.

## 5. Virtual Machine

Stack-based interpreter that executes bytecode.` },
            {
                id: 'opcodes', title: 'Bytecode Reference', content: `# Bytecode Opcodes

The VM supports 17 bytecode instructions:

## Stack Operations
| Opcode | Description |
|--------|-------------|
| CONST | Push constant onto stack |
| POP | Pop and discard top |

## Variable Operations
| Opcode | Description |
|--------|-------------|
| LOAD | Load local variable |
| STORE | Store to local variable |

## Arithmetic
| Opcode | Description |
|--------|-------------|
| ADD | Addition |
| SUB | Subtraction |
| MUL | Multiplication |
| DIV | Division |
| MOD | Modulo |
| NEG | Negate |

## Comparison
| Opcode | Description |
|--------|-------------|
| EQ | Equal |
| NEQ | Not equal |
| LT | Less than |
| GT | Greater than |

## Control Flow
| Opcode | Description |
|--------|-------------|
| JUMP | Unconditional jump |
| JUMP_IF_ZERO | Jump if false |
| CALL | Call function |
| RETURN | Return from function |

## I/O
| Opcode | Description |
|--------|-------------|
| PRINT | Print to output |

## Arrays
| Opcode | Description |
|--------|-------------|
| BUILD_ARRAY | Create array |
| ARRAY_LOAD | Load element |
| ARRAY_STORE | Store element |` },
        ]
    },
    {
        title: 'API',
        items: [
            {
                id: 'rest-api', title: 'REST API', content: `# REST API Reference

Base URL: \`http://57.159.30.64/api\`

---

## POST /compile

Compile and execute source code.

**Request:**
\`\`\`json
{
  "code": "print(42);"
}
\`\`\`

**Success Response:**
\`\`\`json
{
  "success": true,
  "output": "42\\n",
  "executionTime": 5
}
\`\`\`

**Error Response:**
\`\`\`json
{
  "success": false,
  "error": "Parser error: Expected ';' at line 1"
}
\`\`\`

---

## GET /health

Check API status.

**Response:**
\`\`\`json
{
  "status": "ok",
  "timestamp": "2024-01-01T00:00:00Z",
  "compiler": "available"
}
\`\`\`

---

## GET /examples

Get example code snippets.

**Response:**
\`\`\`json
{
  "examples": [
    { "name": "Hello World", "code": "print(\\"Hello!\\");" }
  ]
}
\`\`\`` },
        ]
    },
];

export default function DocsPage() {
    const [activeDoc, setActiveDoc] = useState(DOCS_SECTIONS[0].items[0]);
    const [searchQuery, setSearchQuery] = useState('');

    const filteredSections = DOCS_SECTIONS.map(section => ({
        ...section,
        items: section.items.filter(item =>
            item.title.toLowerCase().includes(searchQuery.toLowerCase()) ||
            item.content.toLowerCase().includes(searchQuery.toLowerCase())
        )
    })).filter(section => section.items.length > 0);

    const renderContent = (content: string) => {
        return content.split('\n').map((line, i) => {
            if (line.startsWith('# ')) {
                return <h1 key={i} className="text-4xl font-bold text-white mb-6 mt-0">{line.slice(2)}</h1>;
            }
            if (line.startsWith('## ')) {
                return <h2 key={i} className="text-2xl font-semibold text-blue-400 mb-4 mt-10 first:mt-0">{line.slice(3)}</h2>;
            }
            if (line.startsWith('### ')) {
                return <h3 key={i} className="text-lg font-semibold text-purple-400 mb-3 mt-6">{line.slice(4)}</h3>;
            }
            if (line.startsWith('```')) {
                return null;
            }
            if (line.startsWith('|')) {
                return <p key={i} className="font-mono text-xs text-gray-400 py-1 border-b border-white/5">{line}</p>;
            }
            if (line.startsWith('---')) {
                return <hr key={i} className="border-white/10 my-8" />;
            }
            if (line.startsWith('- **')) {
                const match = line.match(/- \*\*(.+?)\*\*: (.+)/);
                if (match) {
                    return (
                        <p key={i} className="text-gray-400 my-2 pl-4 border-l-2 border-blue-500/30">
                            <span className="text-white font-medium">{match[1]}</span>: {match[2]}
                        </p>
                    );
                }
            }
            if (line.trim() === '') {
                return <div key={i} className="h-2" />;
            }
            return <p key={i} className="text-gray-400 leading-relaxed my-1">{line}</p>;
        });
    };

    return (
        <div className="h-screen bg-[#0a0a0f] text-white flex overflow-hidden">
            {/* Sidebar */}
            <aside className="w-72 bg-[#12121a] border-r border-white/5 flex flex-col flex-shrink-0">
                <div className="p-4 border-b border-white/5">
                    <Link href="/" className="flex items-center gap-3 hover:opacity-80 transition-opacity mb-4">
                        <div className="w-9 h-9 bg-gradient-to-br from-blue-500 to-purple-500 rounded-xl flex items-center justify-center">
                            <span className="text-white font-bold text-sm">BC</span>
                        </div>
                        <div>
                            <span className="font-bold">Documentation</span>
                            <span className="text-gray-500 text-xs block">v1.0.0</span>
                        </div>
                    </Link>
                    <input
                        type="text"
                        placeholder="Search docs..."
                        value={searchQuery}
                        onChange={(e) => setSearchQuery(e.target.value)}
                        className="w-full px-3 py-2 bg-white/5 border border-white/10 rounded-lg text-sm focus:outline-none focus:border-blue-500/50 transition-colors"
                    />
                </div>

                <nav className="flex-1 overflow-y-auto p-4">
                    {filteredSections.map((section, i) => (
                        <div key={i} className="mb-6">
                            <h3 className="text-xs font-bold text-gray-500 uppercase tracking-wider mb-3 px-2">
                                {section.title}
                            </h3>
                            <ul className="space-y-1">
                                {section.items.map((item) => (
                                    <li key={item.id}>
                                        <button
                                            onClick={() => setActiveDoc(item)}
                                            className={`w-full text-left px-3 py-2 rounded-lg text-sm transition-all ${activeDoc.id === item.id
                                                    ? 'bg-gradient-to-r from-blue-500/20 to-purple-500/20 text-white border-l-2 border-blue-500'
                                                    : 'text-gray-400 hover:text-white hover:bg-white/5'
                                                }`}
                                        >
                                            {item.title}
                                        </button>
                                    </li>
                                ))}
                            </ul>
                        </div>
                    ))}
                </nav>

                <div className="p-4 border-t border-white/5 space-y-2">
                    <Link
                        href="/compiler"
                        className="w-full px-4 py-2.5 bg-gradient-to-r from-blue-600 to-purple-600 hover:from-blue-500 hover:to-purple-500 rounded-xl text-center text-sm font-semibold block transition-all shadow-lg shadow-purple-500/20"
                    >
                        ▶ Open Compiler
                    </Link>
                    <a
                        href="https://github.com/IsVohi/ByteCode-Compiler"
                        target="_blank"
                        className="w-full px-4 py-2 bg-white/5 hover:bg-white/10 border border-white/10 rounded-lg text-center text-sm block transition-colors"
                    >
                        View on GitHub
                    </a>
                </div>
            </aside>

            {/* Main Content */}
            <main className="flex-1 overflow-y-auto">
                <div className="max-w-3xl mx-auto px-8 py-12">
                    <article className="prose prose-invert max-w-none">
                        {renderContent(activeDoc.content)}
                    </article>
                </div>
            </main>
        </div>
    );
}
