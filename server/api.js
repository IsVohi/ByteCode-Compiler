const express = require('express');
const cors = require('cors');
const bodyParser = require('body-parser');
const { exec } = require('child_process');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(cors());
app.use(bodyParser.json({ limit: '1mb' }));

// Configuration
const COMPILER_PATH = path.join(__dirname, '..', 'build', 'compiler');
const TEMP_DIR = '/tmp/compiler';
const TIMEOUT = 5000; // 5 seconds
const MAX_OUTPUT = 10000; // 10KB

// Create temp directory
if (!fs.existsSync(TEMP_DIR)) {
    fs.mkdirSync(TEMP_DIR, { recursive: true });
}

// Health check
app.get('/api/health', (req, res) => {
    res.json({
        status: 'ok',
        timestamp: new Date().toISOString(),
        compiler: fs.existsSync(COMPILER_PATH) ? 'available' : 'missing'
    });
});

// Compile and execute code
app.post('/api/compile', async (req, res) => {
    const { code } = req.body;

    if (!code || typeof code !== 'string') {
        return res.status(400).json({
            success: false,
            error: 'Code is required'
        });
    }

    if (code.length > 50000) {
        return res.status(400).json({
            success: false,
            error: 'Code too large (max 50KB)'
        });
    }

    const timestamp = Date.now();
    const tempFile = path.join(TEMP_DIR, `code_${timestamp}.src`);

    try {
        // Write code to temp file
        fs.writeFileSync(tempFile, code, 'utf8');

        // Execute compiler with timeout
        const child = exec(
            `${COMPILER_PATH} ${tempFile}`,
            {
                timeout: TIMEOUT,
                maxBuffer: MAX_OUTPUT
            },
            (error, stdout, stderr) => {
                // Clean up temp file
                try {
                    fs.unlinkSync(tempFile);
                } catch (e) {
                    console.error('Failed to delete temp file:', e);
                }

                if (error) {
                    // Check if timeout
                    if (error.killed) {
                        return res.json({
                            success: false,
                            error: 'Execution timeout (max 5 seconds)',
                            timeout: true
                        });
                    }

                    // Compilation or runtime error
                    return res.json({
                        success: false,
                        error: stderr || error.message,
                        exitCode: error.code
                    });
                }

                // Success
                res.json({
                    success: true,
                    output: stdout,
                    executionTime: Date.now() - timestamp
                });
            }
        );

    } catch (error) {
        // Clean up on exception
        try {
            if (fs.existsSync(tempFile)) {
                fs.unlinkSync(tempFile);
            }
        } catch (e) { }

        res.status(500).json({
            success: false,
            error: 'Internal server error: ' + error.message
        });
    }
});

// REPL session endpoint (for future interactive REPL)
app.post('/api/repl', (req, res) => {
    res.status(501).json({
        success: false,
        error: 'REPL sessions not yet implemented'
    });
});

// Example code endpoint
app.get('/api/examples', (req, res) => {
    res.json({
        examples: [
            {
                name: 'Hello World',
                code: 'print("Hello, World!");'
            },
            {
                name: 'Factorial',
                code: `fn factorial(n) {
    if (n <= 1) { return 1; }
    return n * factorial(n - 1);
}
print(factorial(5));`
            },
            {
                name: 'Arrays',
                code: `let arr = [1, 2, 3, 4, 5];
arr[2] = 99;
print(arr);`
            },
            {
                name: 'For Loop',
                code: `for (let i = 0; i < 5; i = i + 1) {
    print(i * 2);
}`
            }
        ]
    });
});

// Error handling
app.use((err, req, res, next) => {
    console.error('Error:', err);
    res.status(500).json({
        success: false,
        error: 'Internal server error'
    });
});

// Start server
app.listen(PORT, () => {
    console.log(`ByteCode Compiler API running on port ${PORT}`);
    console.log(`Compiler path: ${COMPILER_PATH}`);
    console.log(`Compiler exists: ${fs.existsSync(COMPILER_PATH)}`);
});
