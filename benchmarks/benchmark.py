#!/usr/bin/env python3
"""
Benchmark runner for the bytecode compiler.
Runs benchmarks with and without optimization and measures timing.
"""

import subprocess
import time
import os
import csv
from pathlib import Path

# Configuration
COMPILER = "../build/compiler"
BENCHMARKS = ["fib.src", "factorial.src", "sum_loop.src"]
ITERATIONS = 5

def run_benchmark(source_file, optimize=True):
    """Run a single benchmark and return the elapsed time in ms."""
    args = [COMPILER, source_file]
    if not optimize:
        args.append("--no-opt")
    args.append("--profile")
    
    start = time.perf_counter()
    try:
        result = subprocess.run(args, capture_output=True, text=True, timeout=30)
        elapsed = (time.perf_counter() - start) * 1000  # Convert to ms
        
        if result.returncode != 0:
            print(f"Error running {source_file}: {result.stderr}")
            return None
        
        return elapsed
    except subprocess.TimeoutExpired:
        print(f"Timeout running {source_file}")
        return None

def main():
    os.chdir(Path(__file__).parent)
    
    results = []
    
    for benchmark in BENCHMARKS:
        if not os.path.exists(benchmark):
            print(f"Warning: {benchmark} not found, skipping")
            continue
        
        print(f"Running {benchmark}...")
        
        # Warm up
        run_benchmark(benchmark, True)
        
        # With optimization
        opt_times = []
        for _ in range(ITERATIONS):
            t = run_benchmark(benchmark, True)
            if t is not None:
                opt_times.append(t)
        
        # Without optimization
        no_opt_times = []
        for _ in range(ITERATIONS):
            t = run_benchmark(benchmark, False)
            if t is not None:
                no_opt_times.append(t)
        
        if opt_times and no_opt_times:
            opt_avg = sum(opt_times) / len(opt_times)
            no_opt_avg = sum(no_opt_times) / len(no_opt_times)
            speedup = no_opt_avg / opt_avg if opt_avg > 0 else 1.0
            
            results.append({
                "benchmark": benchmark,
                "optimized_ms": round(opt_avg, 3),
                "unoptimized_ms": round(no_opt_avg, 3),
                "speedup": round(speedup, 2)
            })
            
            print(f"  Optimized: {opt_avg:.3f}ms, Unoptimized: {no_opt_avg:.3f}ms, Speedup: {speedup:.2f}x")
    
    # Write CSV
    csv_path = "../results/benchmarks.csv"
    os.makedirs(os.path.dirname(csv_path), exist_ok=True)
    
    with open(csv_path, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=["benchmark", "optimized_ms", "unoptimized_ms", "speedup"])
        writer.writeheader()
        writer.writerows(results)
    
    print(f"\nResults written to {csv_path}")

if __name__ == "__main__":
    main()
