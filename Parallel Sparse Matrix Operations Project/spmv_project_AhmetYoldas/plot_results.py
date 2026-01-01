#!/usr/bin/env python3
"""
SpMV Results Visualization
Generates 4 plots from benchmark results
"""

import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import sys

# Set style
plt.style.use('seaborn-v0_8-darkgrid')
colors = ['#3498db', '#e74c3c', '#2ecc71', '#f39c12', '#9b59b6']

def load_results():
    """Load results from CSV"""
    try:
        df = pd.read_csv('results.csv')
        return df
    except FileNotFoundError:
        print("Error: results.csv not found!")
        print("Please run the benchmark first: ./benchmark")
        sys.exit(1)

def plot_performance(df):
    """Plot 1: Performance Comparison (GFlop/s)"""
    fig, ax = plt.subplots(figsize=(12, 7))
    
    methods = df['Method'].values
    gflops = df['GFlops'].values
    
    bars = ax.bar(range(len(methods)), gflops, color=colors, alpha=0.8, edgecolor='black', linewidth=1.5)
    
    # Highlight best method
    best_idx = np.argmax(gflops)
    bars[best_idx].set_color('#e74c3c')
    bars[best_idx].set_alpha(1.0)
    bars[best_idx].set_edgecolor('darkred')
    bars[best_idx].set_linewidth(2.5)
    
    ax.set_xlabel('Method', fontsize=14, fontweight='bold')
    ax.set_ylabel('Performance (GFlop/s)', fontsize=14, fontweight='bold')
    ax.set_title('SpMV Performance Comparison\n5 Methods Tested', fontsize=16, fontweight='bold', pad=20)
    ax.set_xticks(range(len(methods)))
    ax.set_xticklabels(methods, rotation=45, ha='right', fontsize=11)
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    
    # Add value labels on bars
    for i, (bar, val) in enumerate(zip(bars, gflops)):
        height = bar.get_height()
        label = f'{val:.2f}'
        if i == best_idx:
            label += '\n★ BEST'
        ax.text(bar.get_x() + bar.get_width()/2., height,
                label, ha='center', va='bottom', fontsize=11, fontweight='bold')
    
    # Add roofline reference
    roofline = 6.5
    ax.axhline(y=roofline, color='red', linestyle='--', linewidth=2, alpha=0.7, label=f'Roofline Peak ({roofline} GFlop/s)')
    ax.legend(fontsize=11, loc='upper right')
    
    plt.tight_layout()
    plt.savefig('performance_comparison.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: performance_comparison.png")
    plt.close()

def plot_speedup(df):
    """Plot 2: Speedup Comparison"""
    fig, ax = plt.subplots(figsize=(12, 7))
    
    methods = df['Method'].values
    speedups = df['Speedup'].values
    
    bars = ax.bar(range(len(methods)), speedups, color=colors, alpha=0.8, edgecolor='black', linewidth=1.5)
    
    # Highlight best
    best_idx = np.argmax(speedups)
    bars[best_idx].set_color('#e74c3c')
    bars[best_idx].set_alpha(1.0)
    bars[best_idx].set_edgecolor('darkred')
    bars[best_idx].set_linewidth(2.5)
    
    ax.set_xlabel('Method', fontsize=14, fontweight='bold')
    ax.set_ylabel('Speedup vs Baseline', fontsize=14, fontweight='bold')
    ax.set_title('SpMV Speedup Comparison\nRelative to CSR Serial Baseline', fontsize=16, fontweight='bold', pad=20)
    ax.set_xticks(range(len(methods)))
    ax.set_xticklabels(methods, rotation=45, ha='right', fontsize=11)
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    
    # Baseline reference line
    ax.axhline(y=1.0, color='gray', linestyle='--', linewidth=2, alpha=0.5, label='Baseline (1.0×)')
    
    # Add value labels
    for i, (bar, val) in enumerate(zip(bars, speedups)):
        height = bar.get_height()
        label = f'{val:.2f}×'
        if i == best_idx:
            label += '\n★ BEST'
        ax.text(bar.get_x() + bar.get_width()/2., height,
                label, ha='center', va='bottom', fontsize=11, fontweight='bold')
    
    ax.legend(fontsize=11, loc='upper right')
    
    plt.tight_layout()
    plt.savefig('speedup_comparison.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: speedup_comparison.png")
    plt.close()

def plot_time(df):
    """Plot 3: Execution Time Comparison"""
    fig, ax = plt.subplots(figsize=(12, 7))
    
    methods = df['Method'].values
    times = df['Time(ms)'].values
    
    bars = ax.bar(range(len(methods)), times, color=colors, alpha=0.8, edgecolor='black', linewidth=1.5)
    
    # Highlight fastest (lowest time)
    best_idx = np.argmin(times)
    bars[best_idx].set_color('#2ecc71')
    bars[best_idx].set_alpha(1.0)
    bars[best_idx].set_edgecolor('darkgreen')
    bars[best_idx].set_linewidth(2.5)
    
    ax.set_xlabel('Method', fontsize=14, fontweight='bold')
    ax.set_ylabel('Execution Time (ms)', fontsize=14, fontweight='bold')
    ax.set_title('SpMV Execution Time\nLower is Better', fontsize=16, fontweight='bold', pad=20)
    ax.set_xticks(range(len(methods)))
    ax.set_xticklabels(methods, rotation=45, ha='right', fontsize=11)
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    
    # Add value labels
    for i, (bar, val) in enumerate(zip(bars, times)):
        height = bar.get_height()
        label = f'{val:.3f} ms'
        if i == best_idx:
            label += '\n★ FASTEST'
        ax.text(bar.get_x() + bar.get_width()/2., height,
                label, ha='center', va='bottom', fontsize=10, fontweight='bold')
    
    plt.tight_layout()
    plt.savefig('time_comparison.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: time_comparison.png")
    plt.close()

def plot_roofline(df):
    """Plot 4: Roofline Analysis"""
    fig, ax = plt.subplots(figsize=(12, 8))
    
    methods = df['Method'].values
    gflops = df['GFlops'].values
    
    # Roofline model parameters
    peak_bandwidth = 52.0  # GB/s
    arithmetic_intensity = 0.125  # flops/byte for SpMV
    roofline_peak = peak_bandwidth * arithmetic_intensity
    
    # Plot roofline
    ai_range = np.logspace(-2, 2, 100)
    roofline_perf = np.minimum(peak_bandwidth * ai_range, roofline_peak)
    ax.plot(ai_range, roofline_perf, 'r--', linewidth=3, label='Roofline Model', alpha=0.7)
    
    # Plot methods
    for i, (method, perf) in enumerate(zip(methods, gflops)):
        marker = 'o' if i != 1 else '*'  # Star for best method
        size = 150 if i != 1 else 400
        ax.scatter([arithmetic_intensity], [perf], s=size, color=colors[i], 
                  edgecolor='black', linewidth=2, marker=marker, label=method, zorder=10, alpha=0.9)
    
    # Shade attainable region
    ax.fill_between(ai_range, 0, roofline_perf, alpha=0.1, color='red')
    
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_xlabel('Arithmetic Intensity (FLOPs/Byte)', fontsize=14, fontweight='bold')
    ax.set_ylabel('Performance (GFlop/s)', fontsize=14, fontweight='bold')
    ax.set_title('Roofline Analysis\nSpMV Performance vs Hardware Limits', fontsize=16, fontweight='bold', pad=20)
    ax.grid(True, which='both', alpha=0.3, linestyle='--')
    ax.legend(fontsize=10, loc='lower right', ncol=2)
    
    # Add annotations
    ax.axhline(y=roofline_peak, color='red', linestyle=':', linewidth=1, alpha=0.5)
    ax.text(0.5, roofline_peak * 1.1, f'Peak: {roofline_peak:.2f} GFlop/s', 
            fontsize=11, color='red', fontweight='bold')
    
    # Efficiency annotation
    best_idx = np.argmax(gflops)
    efficiency = 100.0 * gflops[best_idx] / roofline_peak
    ax.text(0.01, 0.5, f'Best Efficiency:\n{efficiency:.1f}%', 
            fontsize=12, fontweight='bold', 
            bbox=dict(boxstyle='round', facecolor='yellow', alpha=0.7),
            transform=ax.transAxes)
    
    plt.tight_layout()
    plt.savefig('roofline_analysis.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: roofline_analysis.png")
    plt.close()

def create_summary_figure(df):
    """Plot 5: Combined Summary (2x2 grid)"""
    fig = plt.figure(figsize=(16, 12))
    
    methods = df['Method'].values
    gflops = df['GFlops'].values
    speedups = df['Speedup'].values
    times = df['Time(ms)'].values
    
    # Performance bar chart
    ax1 = plt.subplot(2, 2, 1)
    bars1 = ax1.bar(range(len(methods)), gflops, color=colors, alpha=0.8, edgecolor='black')
    best_idx = np.argmax(gflops)
    bars1[best_idx].set_color('#e74c3c')
    ax1.set_title('Performance (GFlop/s)', fontsize=14, fontweight='bold')
    ax1.set_xticks(range(len(methods)))
    ax1.set_xticklabels(methods, rotation=45, ha='right', fontsize=9)
    ax1.grid(axis='y', alpha=0.3)
    for bar, val in zip(bars1, gflops):
        ax1.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                f'{val:.2f}', ha='center', va='bottom', fontsize=9, fontweight='bold')
    
    # Speedup bar chart
    ax2 = plt.subplot(2, 2, 2)
    bars2 = ax2.bar(range(len(methods)), speedups, color=colors, alpha=0.8, edgecolor='black')
    best_idx_speedup = np.argmax(speedups)
    bars2[best_idx_speedup].set_color('#e74c3c')
    ax2.set_title('Speedup vs Baseline', fontsize=14, fontweight='bold')
    ax2.set_xticks(range(len(methods)))
    ax2.set_xticklabels(methods, rotation=45, ha='right', fontsize=9)
    ax2.axhline(y=1.0, color='gray', linestyle='--', alpha=0.5)
    ax2.grid(axis='y', alpha=0.3)
    for bar, val in zip(bars2, speedups):
        ax2.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                f'{val:.2f}×', ha='center', va='bottom', fontsize=9, fontweight='bold')
    
    # Time bar chart
    ax3 = plt.subplot(2, 2, 3)
    bars3 = ax3.bar(range(len(methods)), times, color=colors, alpha=0.8, edgecolor='black')
    best_idx_time = np.argmin(times)
    bars3[best_idx_time].set_color('#2ecc71')
    ax3.set_title('Execution Time (ms)', fontsize=14, fontweight='bold')
    ax3.set_xticks(range(len(methods)))
    ax3.set_xticklabels(methods, rotation=45, ha='right', fontsize=9)
    ax3.grid(axis='y', alpha=0.3)
    for bar, val in zip(bars3, times):
        ax3.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                f'{val:.3f}', ha='center', va='bottom', fontsize=9, fontweight='bold')
    
    # Efficiency comparison (%)
    ax4 = plt.subplot(2, 2, 4)
    roofline = 6.5
    efficiencies = (gflops / roofline) * 100
    bars4 = ax4.bar(range(len(methods)), efficiencies, color=colors, alpha=0.8, edgecolor='black')
    bars4[best_idx].set_color('#e74c3c')
    ax4.set_title('Roofline Efficiency (%)', fontsize=14, fontweight='bold')
    ax4.set_xticks(range(len(methods)))
    ax4.set_xticklabels(methods, rotation=45, ha='right', fontsize=9)
    ax4.axhline(y=100, color='red', linestyle='--', alpha=0.5, label='100% (Peak)')
    ax4.axhline(y=70, color='orange', linestyle='--', alpha=0.5, label='70% (Target)')
    ax4.grid(axis='y', alpha=0.3)
    ax4.legend(fontsize=8, loc='upper right')
    for bar, val in zip(bars4, efficiencies):
        ax4.text(bar.get_x() + bar.get_width()/2., bar.get_height(),
                f'{val:.1f}%', ha='center', va='bottom', fontsize=9, fontweight='bold')
    
    fig.suptitle('SpMV Complete Performance Summary\n5 Methods Comparison', 
                 fontsize=18, fontweight='bold', y=0.995)
    
    plt.tight_layout(rect=[0, 0, 1, 0.98])
    plt.savefig('summary_all.png', dpi=300, bbox_inches='tight')
    print("✓ Saved: summary_all.png")
    plt.close()

def main():
    print("\n" + "="*50)
    print("SpMV Results Visualization")
    print("="*50 + "\n")
    
    # Load data
    df = load_results()
    print(f"Loaded {len(df)} methods from results.csv\n")
    
    print("Generating plots...\n")
    
    # Generate all plots
    plot_performance(df)
    plot_speedup(df)
    plot_time(df)
    plot_roofline(df)
    create_summary_figure(df)
    
    print("\n" + "="*50)
    print("✓ All plots generated successfully!")
    print("="*50)
    print("\nGenerated files:")
    print("  1. performance_comparison.png - Performance bar chart")
    print("  2. speedup_comparison.png     - Speedup bar chart")
    print("  3. time_comparison.png        - Execution time chart")
    print("  4. roofline_analysis.png      - Roofline model plot")
    print("  5. summary_all.png            - Combined summary (2×2)")
    print("\n")

if __name__ == "__main__":
    main()
