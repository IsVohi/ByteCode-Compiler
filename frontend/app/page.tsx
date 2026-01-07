'use client';

import Link from 'next/link';
import { useState, useEffect, useMemo, useCallback, memo } from 'react';

const FEATURES = [
    { icon: '⚡', title: '17 Opcodes', desc: 'Complete instruction set' },
    { icon: '🔄', title: 'Recursion', desc: 'Full function support' },
    { icon: '📦', title: 'Arrays', desc: 'Dynamic data structures' },
    { icon: '🔧', title: 'Optimizer', desc: 'Constant folding & DCE' },
    { icon: '🎯', title: 'Type System', desc: 'Ints, strings, arrays' },
    { icon: '🚀', title: 'Fast VM', desc: 'Stack-based execution' },
];

const PIPELINE = ['Source', 'Lexer', 'Parser', 'AST', 'Optimizer', 'CodeGen', 'VM'];

// Memoized Feature Card
const FeatureCard = memo(function FeatureCard({ icon, title, desc }: { icon: string; title: string; desc: string }) {
    return (
        <div className="group p-5 rounded-2xl bg-gradient-to-br from-white/5 to-white/[0.02] border border-white/5 hover:border-white/10 transition-all hover:transform hover:-translate-y-1">
            <div className="text-3xl mb-3 group-hover:scale-110 transition-transform">{icon}</div>
            <div className="font-semibold text-white mb-1">{title}</div>
            <div className="text-sm text-gray-500">{desc}</div>
        </div>
    );
});

// Memoized Pipeline Step
const PipelineStep = memo(function PipelineStep({ step, isActive, isPast }: { step: string; isActive: boolean; isPast: boolean }) {
    return (
        <div className={`px-3 py-1.5 rounded-lg transition-all duration-300 ${isActive
            ? 'bg-gradient-to-r from-blue-500 to-purple-500 text-white scale-110 shadow-lg shadow-purple-500/30'
            : isPast
                ? 'bg-gray-700/50 text-gray-400'
                : 'bg-gray-800/30 text-gray-600'
            }`}>
            {step}
        </div>
    );
});

export default function Home() {
    const [mounted, setMounted] = useState(false);
    const [activeStep, setActiveStep] = useState(0);

    useEffect(() => {
        setMounted(true);
    }, []);

    useEffect(() => {
        if (!mounted) return;
        const interval = setInterval(() => {
            setActiveStep((prev) => (prev + 1) % PIPELINE.length);
        }, 1500);
        return () => clearInterval(interval);
    }, [mounted]);

    // Memoize pipeline rendering
    const pipelineElements = useMemo(() => (
        PIPELINE.map((step, i) => (
            <div key={step} className="flex items-center gap-2">
                <PipelineStep step={step} isActive={i === activeStep} isPast={i < activeStep} />
                {i < PIPELINE.length - 1 && (
                    <span className={`transition-colors duration-300 ${i < activeStep ? 'text-purple-400' : 'text-gray-700'}`}>→</span>
                )}
            </div>
        ))
    ), [activeStep]);

    // Memoize features
    const featureElements = useMemo(() => (
        FEATURES.map((feature, i) => (
            <FeatureCard key={i} {...feature} />
        ))
    ), []);

    // Show skeleton while loading
    if (!mounted) {
        return (
            <div className="min-h-screen bg-[#0a0a0f] flex items-center justify-center">
                <div className="w-12 h-12 border-4 border-blue-500 border-t-transparent rounded-full animate-spin" />
            </div>
        );
    }

    return (
        <div className="min-h-screen bg-[#0a0a0f] text-white overflow-x-hidden">
            {/* Optimized Background - using CSS instead of animated divs */}
            <div className="fixed inset-0 pointer-events-none bg-[radial-gradient(ellipse_at_top_left,_rgba(59,130,246,0.1)_0%,_transparent_50%),radial-gradient(ellipse_at_bottom_right,_rgba(168,85,247,0.1)_0%,_transparent_50%)]" />

            {/* Header */}
            <header className="fixed top-0 w-full z-50 backdrop-blur-xl bg-[#0a0a0f]/80 border-b border-white/5">
                <nav className="max-w-6xl mx-auto px-6 py-4 flex justify-between items-center">
                    <Link href="/" className="flex items-center gap-3 group">
                        <div className="w-10 h-10 bg-gradient-to-br from-blue-500 via-purple-500 to-pink-500 rounded-xl flex items-center justify-center shadow-lg shadow-purple-500/25">
                            <span className="text-white font-bold">BC</span>
                        </div>
                        <div>
                            <span className="font-bold text-lg">ByteCode</span>
                            <span className="text-gray-500 text-sm block -mt-1">Compiler</span>
                        </div>
                    </Link>
                    <div className="flex items-center gap-6">
                        <Link href="/docs" className="text-gray-400 hover:text-white transition-colors text-sm">Docs</Link>
                        <a href="https://github.com/IsVohi/ByteCode-Compiler" target="_blank" rel="noopener" className="text-gray-400 hover:text-white transition-colors text-sm">GitHub</a>
                        <a href="#" className="text-gray-400 hover:text-white transition-colors text-sm">Portfolio</a>
                    </div>
                </nav>
            </header>

            {/* Hero */}
            <main className="relative pt-28 pb-16 px-6">
                <section className="max-w-5xl mx-auto text-center">
                    <div className="inline-flex items-center gap-3 px-4 py-2 rounded-full bg-gradient-to-r from-green-500/10 to-blue-500/10 border border-green-500/20 mb-6">
                        <span className="relative flex h-2 w-2">
                            <span className="animate-ping absolute inline-flex h-full w-full rounded-full bg-green-400 opacity-75" />
                            <span className="relative inline-flex rounded-full h-2 w-2 bg-green-500" />
                        </span>
                        <span className="text-sm text-gray-300">Live on Azure • 150+ Tests Passing</span>
                    </div>

                    <h1 className="text-6xl md:text-8xl font-bold mb-4 leading-tight">
                        <span className="bg-gradient-to-r from-white via-blue-100 to-white bg-clip-text text-transparent">ByteCode</span>
                        <br />
                        <span className="bg-gradient-to-r from-blue-400 via-purple-400 to-pink-400 bg-clip-text text-transparent">Compiler</span>
                    </h1>

                    <p className="text-xl md:text-2xl text-gray-400 mb-4 max-w-2xl mx-auto">
                        A modern, optimizing bytecode compiler built from scratch in C++
                    </p>

                    {/* Pipeline Animation */}
                    <div className="flex flex-wrap justify-center items-center gap-2 mb-8 text-sm">
                        {pipelineElements}
                    </div>

                    {/* CTA */}
                    <div className="flex flex-col sm:flex-row gap-4 justify-center mb-14">
                        <Link href="/compiler" className="group relative px-8 py-4 bg-gradient-to-r from-blue-600 to-purple-600 rounded-xl font-semibold text-lg overflow-hidden transition-all hover:shadow-xl hover:shadow-purple-500/25" prefetch>
                            <span className="relative z-10 flex items-center justify-center gap-2">▶ Open Compiler</span>
                        </Link>
                        <Link href="/docs" className="px-8 py-4 bg-white/5 hover:bg-white/10 border border-white/10 rounded-xl font-semibold text-lg transition-all" prefetch>
                            📚 Read Docs
                        </Link>
                    </div>

                    {/* Features */}
                    <div className="grid grid-cols-2 md:grid-cols-3 gap-4 max-w-3xl mx-auto">
                        {featureElements}
                    </div>
                </section>

                {/* Code Preview */}
                <section className="max-w-4xl mx-auto mt-24">
                    <h2 className="text-2xl font-bold text-center mb-8 text-gray-300">See it in action</h2>
                    <div className="rounded-2xl overflow-hidden border border-white/10 bg-[#12121a] shadow-2xl">
                        <div className="flex items-center gap-2 px-4 py-3 bg-[#1a1a24] border-b border-white/5">
                            <div className="flex gap-2">
                                <div className="w-3 h-3 rounded-full bg-red-500/80" />
                                <div className="w-3 h-3 rounded-full bg-yellow-500/80" />
                                <div className="w-3 h-3 rounded-full bg-green-500/80" />
                            </div>
                            <span className="ml-4 text-sm text-gray-500 font-mono">fibonacci.src</span>
                        </div>
                        <pre className="p-6 font-mono text-sm leading-relaxed text-gray-300">
                            {`fn fib(n) {
    if (n <= 1) { return n; }
    return fib(n - 1) + fib(n - 2);
}

print(fib(10));  // Output: 55`}
                        </pre>
                    </div>
                </section>
            </main>

            {/* Footer */}
            <footer className="border-t border-white/5 py-12 px-6">
                <div className="max-w-6xl mx-auto flex flex-col md:flex-row justify-between items-center gap-6">
                    <div className="flex items-center gap-4">
                        <div className="w-8 h-8 bg-gradient-to-br from-blue-500 to-purple-500 rounded-lg flex items-center justify-center">
                            <span className="text-xs font-bold">BC</span>
                        </div>
                        <div>
                            <p className="text-gray-400 text-sm">Created by <span className="text-white font-medium">Vikas Sharma</span></p>
                            <p className="text-gray-600 text-xs">© 2024 All rights reserved</p>
                        </div>
                    </div>
                    <div className="flex items-center gap-8 text-sm text-gray-500">
                        <a href="https://github.com/IsVohi/ByteCode-Compiler" className="hover:text-white transition-colors">GitHub</a>
                        <Link href="/docs" className="hover:text-white transition-colors">Docs</Link>
                        <Link href="/compiler" className="hover:text-white transition-colors">Compiler</Link>
                    </div>
                </div>
            </footer>
        </div>
    );
}
