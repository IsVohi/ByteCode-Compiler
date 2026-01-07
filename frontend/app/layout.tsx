import type { Metadata } from 'next';
import { Inter, JetBrains_Mono } from 'next/font/google';
import './globals.css';

const inter = Inter({ subsets: ['latin'], variable: '--font-inter' });
const jetbrains = JetBrains_Mono({ subsets: ['latin'], variable: '--font-mono' });

export const metadata: Metadata = {
    title: 'ByteCode Compiler - Online Compiler & REPL',
    description: 'A modern optimizing bytecode compiler with lexer, parser, code generator, optimizer, and virtual machine. Built by Vikas Sharma.',
    keywords: ['compiler', 'bytecode', 'programming', 'code editor', 'REPL'],
};

export default function RootLayout({
    children,
}: {
    children: React.ReactNode;
}) {
    return (
        <html lang="en" className="dark">
            <body className={`${inter.variable} ${jetbrains.variable} font-sans antialiased`}>
                {children}
            </body>
        </html>
    );
}
