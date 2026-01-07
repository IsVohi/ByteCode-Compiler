// Use local API routes to proxy to Azure (avoids HTTPS mixed content issue)
const API_URL = '/api';

export interface CompileResult {
    success: boolean;
    output?: string;
    error?: string;
    executionTime?: number;
}

export async function compileCode(code: string): Promise<CompileResult> {
    try {
        const response = await fetch(`${API_URL}/compile`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ code }),
        });
        return await response.json();
    } catch (error) {
        return { success: false, error: 'Failed to connect to compiler server' };
    }
}

export async function checkHealth(): Promise<boolean> {
    try {
        const response = await fetch(`${API_URL}/health`);
        const data = await response.json();
        return data.status === 'ok';
    } catch {
        return false;
    }
}


export const EXAMPLES = [
    { name: 'Hello World', code: 'print("Hello, World!");' },
    { name: 'Variables', code: 'let x = 10;\nlet y = 20;\nprint(x + y);' },
    { name: 'Factorial', code: 'fn factorial(n) {\n    if (n <= 1) { return 1; }\n    return n * factorial(n - 1);\n}\nprint(factorial(5));' },
    { name: 'Arrays', code: 'let arr = [1, 2, 3, 4, 5];\narr[2] = 99;\nprint(arr);' },
    { name: 'For Loop', code: 'let sum = 0;\nfor (let i = 1; i <= 10; i = i + 1) {\n    sum = sum + i;\n}\nprint(sum);' },
    { name: 'Fibonacci', code: 'fn fib(n) {\n    if (n <= 1) { return n; }\n    return fib(n - 1) + fib(n - 2);\n}\nfor (let i = 0; i < 10; i = i + 1) {\n    print(fib(i));\n}' },
];
