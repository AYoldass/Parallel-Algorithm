#!/usr/bin/env python3
"""
Performance Graph Generator for MPI Image Downscaling
Reads results.csv and analysis.csv, creates performance graphs
"""

import matplotlib.pyplot as plt
import csv
import sys
import os

def read_csv(filename):
    """Read CSV file and return data"""
    data = []
    try:
        with open(filename, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                data.append(row)
        return data
    except FileNotFoundError:
        print(f"Error: {filename} not found!")
        print(f"Please run benchmark.sh first.")
        sys.exit(1)

def plot_execution_times(results_data):
    """Plot execution times comparison"""
    processes = []
    times = []
    
    for row in results_data:
        proc = row['Process']
        avg_time = float(row['Average'])
        
        processes.append(proc)
        times.append(avg_time)
    
    plt.figure(figsize=(10, 6))
    colors = ['#e74c3c', '#3498db', '#2ecc71', '#f39c12', '#9b59b6']
    bars = plt.bar(processes, times, color=colors[:len(processes)], edgecolor='black', linewidth=1.2)
    
    # Add value labels on bars
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width()/2., height,
                f'{height:.6f}s',
                ha='center', va='bottom', fontsize=9, fontweight='bold')
    
    plt.xlabel('Number of Processes', fontsize=12, fontweight='bold')
    plt.ylabel('Execution Time (seconds)', fontsize=12, fontweight='bold')
    plt.title('Execution Time Comparison', fontsize=14, fontweight='bold')
    plt.grid(axis='y', alpha=0.3, linestyle='--')
    plt.tight_layout()
    plt.savefig('execution_times.png', dpi=300, bbox_inches='tight')
    print("✓ Created: execution_times.png")
    plt.close()

def plot_speedup(analysis_data):
    """Plot speedup graph"""
    processes = []
    speedups = []
    
    for row in analysis_data:
        proc = int(row['Process'])
        speedup = float(row['Speedup'])
        
        processes.append(proc)
        speedups.append(speedup)
    
    plt.figure(figsize=(10, 6))
    
    # Ideal speedup line
    max_proc = max(processes)
    ideal = list(range(1, max_proc + 1))
    plt.plot(ideal, ideal, 'r--', linewidth=2, label='Ideal Speedup (Linear)', alpha=0.7)
    
    # Actual speedup
    plt.plot(processes, speedups, 'bo-', linewidth=2.5, markersize=10, 
             label='Actual Speedup', markerfacecolor='lightblue', markeredgewidth=2)
    
    # Add value labels
    for i, (p, s) in enumerate(zip(processes, speedups)):
        plt.text(p, s + 0.2, f'{s:.2f}×', ha='center', fontsize=9, fontweight='bold')
    
    plt.xlabel('Number of Processes', fontsize=12, fontweight='bold')
    plt.ylabel('Speedup', fontsize=12, fontweight='bold')
    plt.title('Speedup Analysis', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11, loc='upper left')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xticks(processes)
    plt.tight_layout()
    plt.savefig('speedup.png', dpi=300, bbox_inches='tight')
    print("✓ Created: speedup.png")
    plt.close()

def plot_efficiency(analysis_data):
    """Plot efficiency graph"""
    processes = []
    efficiencies = []
    
    for row in analysis_data:
        proc = int(row['Process'])
        eff = float(row['Efficiency']) * 100  # Convert to percentage
        
        processes.append(proc)
        efficiencies.append(eff)
    
    plt.figure(figsize=(10, 6))
    
    # 100% efficiency line
    plt.axhline(y=100, color='r', linestyle='--', linewidth=2, label='Ideal Efficiency (100%)', alpha=0.7)
    
    # Actual efficiency
    plt.plot(processes, efficiencies, 'go-', linewidth=2.5, markersize=10,
             label='Actual Efficiency', markerfacecolor='lightgreen', markeredgewidth=2)
    
    # Add value labels
    for p, e in zip(processes, efficiencies):
        plt.text(p, e + 2, f'{e:.1f}%', ha='center', fontsize=9, fontweight='bold')
    
    plt.xlabel('Number of Processes', fontsize=12, fontweight='bold')
    plt.ylabel('Efficiency (%)', fontsize=12, fontweight='bold')
    plt.title('Parallel Efficiency', fontsize=14, fontweight='bold')
    plt.legend(fontsize=11, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xticks(processes)
    plt.ylim(0, 110)
    plt.tight_layout()
    plt.savefig('efficiency.png', dpi=300, bbox_inches='tight')
    print("✓ Created: efficiency.png")
    plt.close()

def plot_combined(analysis_data):
    """Plot speedup and efficiency on same graph (dual axis)"""
    processes = []
    speedups = []
    efficiencies = []
    
    for row in analysis_data:
        proc = int(row['Process'])
        speedup = float(row['Speedup'])
        eff = float(row['Efficiency']) * 100
        
        processes.append(proc)
        speedups.append(speedup)
        efficiencies.append(eff)
    
    fig, ax1 = plt.subplots(figsize=(12, 6))
    
    # Speedup on left axis
    color1 = '#2c3e50'
    ax1.set_xlabel('Number of Processes', fontsize=12, fontweight='bold')
    ax1.set_ylabel('Speedup', color=color1, fontsize=12, fontweight='bold')
    line1 = ax1.plot(processes, speedups, 'o-', color=color1, linewidth=2.5, 
                     markersize=10, label='Speedup', markeredgewidth=2)
    ax1.tick_params(axis='y', labelcolor=color1)
    ax1.grid(True, alpha=0.3, linestyle='--')
    
    # Ideal speedup
    max_proc = max(processes)
    ideal = list(range(1, max_proc + 1))
    ax1.plot(ideal, ideal, '--', color=color1, linewidth=2, alpha=0.5, label='Ideal Speedup')
    
    # Efficiency on right axis
    ax2 = ax1.twinx()
    color2 = '#27ae60'
    ax2.set_ylabel('Efficiency (%)', color=color2, fontsize=12, fontweight='bold')
    line2 = ax2.plot(processes, efficiencies, 's-', color=color2, linewidth=2.5,
                     markersize=10, label='Efficiency', markeredgewidth=2)
    ax2.tick_params(axis='y', labelcolor=color2)
    ax2.axhline(y=100, color=color2, linestyle='--', linewidth=2, alpha=0.5)
    ax2.set_ylim(0, 110)
    
    # Add value labels
    for p, s in zip(processes, speedups):
        ax1.text(p, s + 0.15, f'{s:.2f}×', ha='center', fontsize=9, fontweight='bold')
    for p, e in zip(processes, efficiencies):
        ax2.text(p, e + 2, f'{e:.1f}%', ha='center', fontsize=9, fontweight='bold', color=color2)
    
    # Combined legend
    lines = line1 + line2
    labels = [l.get_label() for l in lines]
    ax1.legend(lines, labels, loc='upper left', fontsize=11)
    
    plt.title('Performance Analysis: Speedup and Efficiency', fontsize=14, fontweight='bold')
    plt.xticks(processes)
    fig.tight_layout()
    plt.savefig('combined_performance.png', dpi=300, bbox_inches='tight')
    print("✓ Created: combined_performance.png")
    plt.close()

def generate_summary_table():
    """Generate a summary image with key statistics"""
    # Read data
    results_data = read_csv('results.csv')
    analysis_data = read_csv('analysis.csv')
    
    # Extract key metrics
    seq_time = float(results_data[0]['Average'])
    best_speedup = max(float(row['Speedup']) for row in analysis_data)
    best_proc = [int(row['Process']) for row in analysis_data 
                 if float(row['Speedup']) == best_speedup][0]
    avg_efficiency = sum(float(row['Efficiency']) for row in analysis_data) / len(analysis_data)
    
    # Create figure
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.axis('tight')
    ax.axis('off')
    
    # Summary data
    summary = [
        ['Sequential Time', f'{seq_time:.6f} seconds'],
        ['Best Speedup', f'{best_speedup:.2f}× (with {best_proc} processes)'],
        ['Average Efficiency', f'{avg_efficiency*100:.1f}%'],
        ['Test Image Size', '4000×3000 pixels'],
        ['Number of Runs', '3 per configuration'],
        ['Algorithm', 'Box Filtering (2×2 averaging)']
    ]
    
    table = ax.table(cellText=summary, cellLoc='left',
                    colWidths=[0.4, 0.6], loc='center',
                    cellColours=[['#ecf0f1', '#ecf0f1']] * len(summary))
    
    table.auto_set_font_size(False)
    table.set_fontsize(12)
    table.scale(1, 2.5)
    
    # Style header
    for i in range(len(summary)):
        table[(i, 0)].set_facecolor('#3498db')
        table[(i, 0)].set_text_props(weight='bold', color='white')
        table[(i, 1)].set_text_props(weight='bold')
    
    plt.title('Performance Summary', fontsize=16, fontweight='bold', pad=20)
    plt.savefig('summary.png', dpi=300, bbox_inches='tight')
    print("✓ Created: summary.png")
    plt.close()

def main():
    print("\n=== Performance Graph Generator ===\n")
    
    # Check if required files exist
    if not os.path.exists('results.csv'):
        print("Error: results.csv not found!")
        print("Please run benchmark.sh first.")
        sys.exit(1)
    
    if not os.path.exists('analysis.csv'):
        print("Error: analysis.csv not found!")
        print("Please run analyze_results.sh first.")
        sys.exit(1)
    
    # Read data
    results_data = read_csv('results.csv')
    analysis_data = read_csv('analysis.csv')
    
    print(f"Found {len(results_data)} configurations")
    print(f"Generating graphs...\n")
    
    # Generate all graphs
    plot_execution_times(results_data)
    plot_speedup(analysis_data)
    plot_efficiency(analysis_data)
    plot_combined(analysis_data)
    generate_summary_table()
    
    print("\n=== All Graphs Created Successfully! ===")
    print("\nGenerated files:")
    print("  1. execution_times.png     - Bar chart of execution times")
    print("  2. speedup.png             - Speedup analysis")
    print("  3. efficiency.png          - Efficiency analysis")
    print("  4. combined_performance.png - Combined speedup & efficiency")
    print("  5. summary.png             - Performance summary table")
    print("\nYou can include these images in your report!")

if __name__ == "__main__":
    main()