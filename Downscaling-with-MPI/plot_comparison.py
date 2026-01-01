#!/usr/bin/env python3
"""
MPI vs OpenMP Performance Comparison Graphs
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
        return None

def plot_mpi_vs_openmp_speedup():
    """Compare speedup between MPI and OpenMP"""
    # Read data
    mpi_data = read_csv('analysis.csv')
    omp_data = read_csv('analysis_openmp.csv')
    
    if not mpi_data or not omp_data:
        print("Warning: Missing data files, skipping comparison graph")
        return
    
    # Extract data
    mpi_workers = [int(row['Process']) for row in mpi_data]
    mpi_speedup = [float(row['Speedup']) for row in mpi_data]
    
    omp_workers = [int(row['Threads']) for row in omp_data]
    omp_speedup = [float(row['Speedup']) for row in omp_data]
    
    plt.figure(figsize=(12, 7))
    
    # Ideal line
    max_workers = max(max(mpi_workers), max(omp_workers))
    ideal = list(range(1, max_workers + 1))
    plt.plot(ideal, ideal, 'k--', linewidth=2, label='Ideal (Linear)', alpha=0.5)
    
    # MPI line
    plt.plot(mpi_workers, mpi_speedup, 'ro-', linewidth=2.5, markersize=10,
             label='MPI', markerfacecolor='lightcoral', markeredgewidth=2)
    
    # OpenMP line
    plt.plot(omp_workers, omp_speedup, 'bs-', linewidth=2.5, markersize=10,
             label='OpenMP', markerfacecolor='lightblue', markeredgewidth=2)
    
    # Add value labels
    for w, s in zip(mpi_workers, mpi_speedup):
        plt.text(w, s + 0.05, f'{s:.2f}', ha='center', fontsize=8, color='red')
    for w, s in zip(omp_workers, omp_speedup):
        plt.text(w, s - 0.15, f'{s:.2f}', ha='center', fontsize=8, color='blue')
    
    plt.xlabel('Number of Workers (Processes/Threads)', fontsize=12, fontweight='bold')
    plt.ylabel('Speedup', fontsize=12, fontweight='bold')
    plt.title('MPI vs OpenMP: Speedup Comparison', fontsize=14, fontweight='bold')
    plt.legend(fontsize=12, loc='upper left')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xticks(ideal)
    plt.tight_layout()
    plt.savefig('mpi_vs_openmp_speedup.png', dpi=300, bbox_inches='tight')
    print("✓ Created: mpi_vs_openmp_speedup.png")
    plt.close()

def plot_mpi_vs_openmp_efficiency():
    """Compare efficiency between MPI and OpenMP"""
    mpi_data = read_csv('analysis.csv')
    omp_data = read_csv('analysis_openmp.csv')
    
    if not mpi_data or not omp_data:
        return
    
    mpi_workers = [int(row['Process']) for row in mpi_data]
    mpi_eff = [float(row['Efficiency']) * 100 for row in mpi_data]
    
    omp_workers = [int(row['Threads']) for row in omp_data]
    omp_eff = [float(row['Efficiency']) * 100 for row in omp_data]
    
    plt.figure(figsize=(12, 7))
    
    # 100% line
    plt.axhline(y=100, color='k', linestyle='--', linewidth=2, alpha=0.5, label='Ideal (100%)')
    
    # MPI line
    plt.plot(mpi_workers, mpi_eff, 'ro-', linewidth=2.5, markersize=10,
             label='MPI', markerfacecolor='lightcoral', markeredgewidth=2)
    
    # OpenMP line
    plt.plot(omp_workers, omp_eff, 'bs-', linewidth=2.5, markersize=10,
             label='OpenMP', markerfacecolor='lightblue', markeredgewidth=2)
    
    # Add value labels
    for w, e in zip(mpi_workers, mpi_eff):
        plt.text(w, e + 3, f'{e:.1f}%', ha='center', fontsize=8, color='red')
    for w, e in zip(omp_workers, omp_eff):
        plt.text(w, e - 8, f'{e:.1f}%', ha='center', fontsize=8, color='blue')
    
    plt.xlabel('Number of Workers (Processes/Threads)', fontsize=12, fontweight='bold')
    plt.ylabel('Efficiency (%)', fontsize=12, fontweight='bold')
    plt.title('MPI vs OpenMP: Efficiency Comparison', fontsize=14, fontweight='bold')
    plt.legend(fontsize=12, loc='upper right')
    plt.grid(True, alpha=0.3, linestyle='--')
    plt.xticks(mpi_workers)
    plt.ylim(0, 110)
    plt.tight_layout()
    plt.savefig('mpi_vs_openmp_efficiency.png', dpi=300, bbox_inches='tight')
    print("✓ Created: mpi_vs_openmp_efficiency.png")
    plt.close()

def plot_execution_time_comparison():
    """Compare raw execution times"""
    mpi_data = read_csv('analysis.csv')
    omp_data = read_csv('analysis_openmp.csv')
    
    if not mpi_data or not omp_data:
        return
    
    mpi_workers = [int(row['Process']) for row in mpi_data]
    mpi_time = [float(row['Average_Time']) for row in mpi_data]
    
    omp_workers = [int(row['Threads']) for row in omp_data]
    omp_time = [float(row['Average_Time']) for row in omp_data]
    
    x = range(len(mpi_workers))
    width = 0.35
    
    fig, ax = plt.subplots(figsize=(12, 7))
    
    bars1 = ax.bar([i - width/2 for i in x], mpi_time, width, 
                   label='MPI', color='coral', edgecolor='black', linewidth=1.2)
    bars2 = ax.bar([i + width/2 for i in x], omp_time, width,
                   label='OpenMP', color='skyblue', edgecolor='black', linewidth=1.2)
    
    # Add value labels
    for bars in [bars1, bars2]:
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{height:.6f}s', ha='center', va='bottom', fontsize=8)
    
    ax.set_xlabel('Number of Workers', fontsize=12, fontweight='bold')
    ax.set_ylabel('Execution Time (seconds)', fontsize=12, fontweight='bold')
    ax.set_title('MPI vs OpenMP: Execution Time Comparison', fontsize=14, fontweight='bold')
    ax.set_xticks(x)
    ax.set_xticklabels(mpi_workers)
    ax.legend(fontsize=12)
    ax.grid(axis='y', alpha=0.3, linestyle='--')
    plt.tight_layout()
    plt.savefig('mpi_vs_openmp_time.png', dpi=300, bbox_inches='tight')
    print("✓ Created: mpi_vs_openmp_time.png")
    plt.close()

def main():
    print("\n=== MPI vs OpenMP Comparison Graphs ===\n")
    
    # Check if files exist
    if not os.path.exists('analysis.csv'):
        print("Error: analysis.csv not found!")
        print("Please run: ./benchmark.sh && ./analyze_results.sh")
        return
    
    if not os.path.exists('analysis_openmp.csv'):
        print("Error: analysis_openmp.csv not found!")
        print("Please run: ./benchmark_openmp.sh && ./analyze_openmp.sh")
        return
    
    print("Generating comparison graphs...\n")
    
    plot_mpi_vs_openmp_speedup()
    plot_mpi_vs_openmp_efficiency()
    plot_execution_time_comparison()
    
    print("\n=== Comparison Graphs Created! ===")
    print("\nGenerated files:")
    print("  1. mpi_vs_openmp_speedup.png    - Speedup comparison")
    print("  2. mpi_vs_openmp_efficiency.png - Efficiency comparison")
    print("  3. mpi_vs_openmp_time.png       - Execution time comparison")
    print("\nYou can include these in your report!")

if __name__ == "__main__":
    main()