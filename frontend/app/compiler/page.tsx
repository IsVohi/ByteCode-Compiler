'use client';

import { useState, useEffect, useRef, useCallback, useMemo, memo } from 'react';
import Link from 'next/link';
import { compileCode, EXAMPLES } from '@/lib/api';

type Theme = { name: string; bg: string; text: string; accent: string };

const THEMES: Theme[] = [
    { name: 'Tokyo Night', bg: '#1a1b26', text: '#a9b1d6', accent: '#7aa2f7' },
    { name: 'Dracula', bg: '#282a36', text: '#f8f8f2', accent: '#bd93f9' },
    { name: 'One Dark', bg: '#282c34', text: '#abb2bf', accent: '#61afef' },
    { name: 'Monokai', bg: '#272822', text: '#f8f8f2', accent: '#f92672' },
    { name: 'Nord', bg: '#2e3440', text: '#d8dee9', accent: '#88c0d0' },
];

// Memoized Example Button
const ExampleButton = memo(function ExampleButton({ name, code, onSelect }: { name: string; code: string; onSelect: (code: string) => void }) {
    return (
        <button
            onClick={() => onSelect(code)}
            className="w-full text-left px-4 py-3 rounded-xl bg-white/[0.03] hover:bg-white/[0.08] border border-white/5 hover:border-white/10 text-sm transition-all"
        >
            <div className="font-medium text-white">{name}</div>
            <div className="text-xs text-gray-500 mt-1 truncate font-mono">{code.split('\n')[0]}</div>
        </button>
    );
});

// Memoized Theme Button
const ThemeButton = memo(function ThemeButton({ theme, isActive, onSelect }: { theme: Theme; isActive: boolean; onSelect: (t: Theme) => void }) {
    return (
        <button
            onClick={() => onSelect(theme)}
            className={`w-full flex items-center gap-3 px-3 py-2 rounded-lg text-sm transition-all ${isActive ? 'bg-blue-500/20 border border-blue-500/30' : 'bg-white/[0.03] border border-white/5 hover:border-white/10'}`}
        >
            <div className="w-4 h-4 rounded" style={{ backgroundColor: theme.bg, border: `2px solid ${theme.accent}` }} />
            <span>{theme.name}</span>
        </button>
    );
});

// Memoized Line Numbers
const LineNumbers = memo(function LineNumbers({ count, fontSize, color }: { count: number; fontSize: number; color: string }) {
    const lines = useMemo(() => Array.from({ length: count }, (_, i) => i + 1), [count]);
    return (
        <div className="py-4 px-3 text-right select-none border-r border-white/5 flex-shrink-0" style={{ color, opacity: 0.3, fontSize }}>
            {lines.map(n => <div key={n} className="leading-6 font-mono">{n}</div>)}
        </div>
    );
});

export default function CompilerPage() {
    const [code, setCode] = useState(EXAMPLES[0].code);
    const [output, setOutput] = useState('');
    const [isRunning, setIsRunning] = useState(false);
    const [panel, setPanel] = useState<'examples' | 'help' | 'settings' | null>(null);
    const [theme, setTheme] = useState(THEMES[0]);
    const [fontSize, setFontSize] = useState(14);
    const [isConnected, setIsConnected] = useState<boolean | null>(null);
    const textareaRef = useRef<HTMLTextAreaElement>(null);

    // Check connection once
    useEffect(() => {
        const controller = new AbortController();
        fetch('/api/health', { signal: controller.signal })
            .then(res => res.json())
            .then(data => setIsConnected(data.status === 'ok'))
            .catch(() => setIsConnected(false));
        return () => controller.abort();
    }, []);

    // Memoized handlers
    const handleRun = useCallback(async () => {
        setIsRunning(true);
        setOutput('Compiling...');
        const result = await compileCode(code);
        setOutput(result.success
            ? `${result.output || '(no output)'}\n\n───────────────────\n✓ ${result.executionTime || 0}ms`
            : `✗ Error\n\n${result.error}`
        );
        setIsRunning(false);
    }, [code]);

    const handleKeyDown = useCallback((e: React.KeyboardEvent) => {
        if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') {
            e.preventDefault();
            handleRun();
        }
        if (e.key === 'Tab') {
            e.preventDefault();
            const target = e.target as HTMLTextAreaElement;
            const start = target.selectionStart;
            setCode(c => c.substring(0, start) + '    ' + c.substring(target.selectionEnd));
            requestAnimationFrame(() => { target.selectionStart = target.selectionEnd = start + 4; });
        }
    }, [handleRun]);

    const handleExampleSelect = useCallback((exampleCode: string) => {
        setCode(exampleCode);
        setPanel(null);
    }, []);

    const handleThemeSelect = useCallback((t: Theme) => setTheme(t), []);

    const togglePanel = useCallback((p: 'examples' | 'help' | 'settings') => {
        setPanel(prev => prev === p ? null : p);
    }, []);

    // Memoized values
    const lineCount = useMemo(() => code.split('\n').length, [code]);
    const outputColor = useMemo(() => output.includes('Error') || output.includes('✗') ? '#f7768e' : '#9ece6a', [output]);

    return (
        <div className="h-screen bg-[#0a0a0f] text-white flex flex-col overflow-hidden">
            {/* Header */}
            <header className="bg-[#12121a] border-b border-white/5 px-4 py-2.5 flex-shrink-0">
                <div className="flex items-center justify-between">
                    <div className="flex items-center gap-4">
                        <Link href="/" className="flex items-center gap-2" prefetch>
                            <div className="w-8 h-8 bg-gradient-to-br from-blue-500 to-purple-500 rounded-lg flex items-center justify-center">
                                <span className="text-white font-bold text-xs">BC</span>
                            </div>
                            <span className="font-semibold">ByteCode</span>
                        </Link>
                        <div className="h-4 w-px bg-white/10" />
                        <span className="text-gray-500 text-sm">Terminal</span>
                    </div>
                    <div className="flex items-center gap-2">
                        {['examples', 'help', 'settings'].map(p => (
                            <button
                                key={p}
                                onClick={() => togglePanel(p as any)}
                                className={`px-3 py-1.5 text-sm rounded-lg transition-all ${panel === p ? 'bg-blue-500/20 text-blue-400' : 'bg-white/5 hover:bg-white/10 text-gray-400'}`}
                            >
                                {p === 'examples' ? '📁' : p === 'help' ? '❓' : '⚙️'} {p === 'examples' ? 'Examples' : p === 'help' ? 'Syntax' : ''}
                            </button>
                        ))}
                        <a href="https://github.com/IsVohi/ByteCode-Compiler" target="_blank" rel="noopener" className="px-3 py-1.5 text-sm bg-white/5 hover:bg-white/10 rounded-lg text-gray-400">GitHub</a>
                    </div>
                </div>
            </header>

            <div className="flex-1 flex overflow-hidden">
                {/* Editor */}
                <div className="flex-1 flex flex-col min-w-0">
                    <div className="bg-[#12121a]/50 border-b border-white/5 px-4 py-2 flex items-center justify-between flex-shrink-0">
                        <div className="flex items-center gap-3">
                            <div className="flex gap-1.5">
                                <div className="w-3 h-3 rounded-full bg-red-500/80" />
                                <div className="w-3 h-3 rounded-full bg-yellow-500/80" />
                                <div className="w-3 h-3 rounded-full bg-green-500/80" />
                            </div>
                            <span className="text-sm text-gray-500 font-mono">main.src</span>
                            <span className="text-xs text-gray-600">{lineCount} lines</span>
                        </div>
                        <div className="flex items-center gap-3">
                            <span className="text-xs text-gray-500 hidden sm:inline">⌘+Enter</span>
                            <button onClick={handleRun} disabled={isRunning} className="px-5 py-1.5 bg-gradient-to-r from-green-500 to-emerald-600 disabled:from-gray-600 disabled:to-gray-600 rounded-lg text-sm font-semibold transition-all flex items-center gap-2 shadow-lg shadow-green-500/20 disabled:shadow-none">
                                {isRunning ? <><span className="w-3 h-3 border-2 border-white border-l-transparent rounded-full animate-spin" />Running</> : '▶ Run'}
                            </button>
                        </div>
                    </div>

                    <div className="flex-1 flex overflow-hidden">
                        <div className="flex-1 flex overflow-hidden" style={{ backgroundColor: theme.bg }}>
                            <LineNumbers count={lineCount} fontSize={fontSize} color={theme.text} />
                            <textarea
                                ref={textareaRef}
                                value={code}
                                onChange={e => setCode(e.target.value)}
                                onKeyDown={handleKeyDown}
                                className="flex-1 p-4 resize-none focus:outline-none font-mono leading-6 bg-transparent"
                                style={{ color: theme.text, fontSize, caretColor: theme.accent }}
                                spellCheck={false}
                            />
                        </div>

                        <div className="w-2/5 border-l border-white/5 flex flex-col flex-shrink-0" style={{ backgroundColor: theme.bg }}>
                            <div className="px-4 py-2 bg-black/20 border-b border-white/5 text-sm text-gray-400 flex items-center justify-between">
                                <span>Output</span>
                                {isConnected !== null && (
                                    <span className={`flex items-center gap-1.5 text-xs ${isConnected ? 'text-green-400' : 'text-red-400'}`}>
                                        <span className={`w-1.5 h-1.5 rounded-full ${isConnected ? 'bg-green-400' : 'bg-red-400'}`} />
                                        {isConnected ? 'Connected' : 'Offline'}
                                    </span>
                                )}
                            </div>
                            <pre className="flex-1 p-4 font-mono overflow-auto whitespace-pre-wrap" style={{ color: outputColor, fontSize }}>
                                {output || 'Click "Run" to execute...'}
                            </pre>
                        </div>
                    </div>
                </div>

                {/* Sidebar - RIGHT SIDE */}
                {panel && (
                    <div className="w-72 bg-[#12121a] border-l border-white/5 flex flex-col flex-shrink-0">
                        <div className="p-4 border-b border-white/5 flex justify-between items-center">
                            <h3 className="font-semibold text-sm capitalize">{panel}</h3>
                            <button onClick={() => setPanel(null)} className="text-gray-500 hover:text-white text-lg">×</button>
                        </div>
                        <div className="flex-1 overflow-y-auto p-4">
                            {panel === 'examples' && (
                                <div className="space-y-2">
                                    {EXAMPLES.map((ex, i) => <ExampleButton key={i} {...ex} onSelect={handleExampleSelect} />)}
                                </div>
                            )}
                            {panel === 'help' && (
                                <div className="space-y-4 text-sm">
                                    {[
                                        { t: 'Variables', c: 'let x = 10;' },
                                        { t: 'Functions', c: 'fn add(a, b) {\n  return a + b;\n}' },
                                        { t: 'If/Else', c: 'if (x > 0) {\n  print(x);\n}' },
                                        { t: 'For Loop', c: 'for (let i = 0; i < 10; i = i + 1) {\n  print(i);\n}' },
                                        { t: 'Arrays', c: 'let arr = [1, 2, 3];\narr[1] = 99;' },
                                    ].map(({ t, c }, i) => (
                                        <div key={i}>
                                            <h4 className="font-semibold text-blue-400 mb-2">{t}</h4>
                                            <pre className="bg-black/30 p-2 rounded text-gray-300 text-xs font-mono whitespace-pre-wrap">{c}</pre>
                                        </div>
                                    ))}
                                </div>
                            )}
                            {panel === 'settings' && (
                                <div className="space-y-6">
                                    <div>
                                        <h4 className="font-semibold text-gray-300 mb-3">Theme</h4>
                                        <div className="space-y-2">
                                            {THEMES.map(t => <ThemeButton key={t.name} theme={t} isActive={theme.name === t.name} onSelect={handleThemeSelect} />)}
                                        </div>
                                    </div>
                                    <div>
                                        <h4 className="font-semibold text-gray-300 mb-3">Font Size: {fontSize}px</h4>
                                        <input type="range" min="10" max="20" value={fontSize} onChange={e => setFontSize(+e.target.value)} className="w-full accent-blue-500" />
                                    </div>
                                </div>
                            )}
                        </div>
                    </div>
                )}
            </div>

            <footer className="bg-[#12121a] border-t border-white/5 px-4 py-1.5 text-xs text-gray-600 flex justify-between flex-shrink-0">
                <span>ByteCode v1.0 • {theme.name}</span>
                <span>Created by Vikas Sharma</span>
            </footer>
        </div>
    );
}
