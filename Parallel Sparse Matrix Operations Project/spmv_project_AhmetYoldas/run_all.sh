#!/bin/bash
# Modular SpMV - Complete Analysis
# Single command: benchmark + visualization

set -e

echo "=========================================="
echo "MODULAR SpMV FINAL ANALYSIS"
echo "1 Serial + 4 Parallel Methods"
echo "=========================================="
echo ""

# Check gcc
if ! command -v gcc &> /dev/null; then
    echo "✗ gcc not found"
    echo "  Install: sudo apt-get install gcc"
    exit 1
fi
echo "✓ gcc: $(gcc --version | head -n1)"

# Check OpenMP
if ! echo | gcc -fopenmp -E - &> /dev/null; then
    echo "✗ OpenMP not found"
    echo "  Install: sudo apt-get install libomp-dev"
    exit 1
fi
echo "✓ OpenMP available"

# Check Python3
if ! command -v python3 &> /dev/null; then
    echo "✗ python3 not found"
    echo "  Install: sudo apt-get install python3"
    exit 1
fi
echo "✓ python3: $(python3 --version)"

# Check matplotlib
if ! python3 -c "import matplotlib" 2>/dev/null; then
    echo "✗ matplotlib not found - installing..."
    pip3 install matplotlib pandas --user
    if [ $? -ne 0 ]; then
        echo "✗ Failed to install matplotlib"
        exit 1
    fi
fi
echo "✓ matplotlib available"

# Check pandas
if ! python3 -c "import pandas" 2>/dev/null; then
    echo "Installing pandas..."
    pip3 install pandas --user
fi
echo "✓ pandas available"

echo ""
echo "=========================================="
echo "MODULAR STRUCTURE"
echo "=========================================="
echo ""
echo "Source files:"
echo "  ✓ common.c/h              - Shared utilities"
echo "  ✓ csr_serial.c/h          - Method 1 (baseline)"
echo "  ✓ csr_parallel.c/h        - Method 2 (OpenMP)"
echo "  ✓ bcsr_parallel.c/h       - Method 3 (4×4 + OpenMP)"
echo "  ✓ bucket_parallel.c/h     - Method 4 (Buckets + OpenMP)"
echo "  ✓ bcsr_bucket_parallel.c/h - Method 5 (Hybrid) ⭐"
echo "  ✓ benchmark.c             - Main program"
echo ""

# Build
echo "Building modular benchmark..."
make clean 2>/dev/null || true
make

if [ $? -ne 0 ]; then
    echo ""
    echo "✗ Build failed!"
    exit 1
fi

echo ""
echo "=========================================="
echo "RUNNING BENCHMARK (5 METHODS)"
echo "=========================================="
echo ""

# Run benchmark
./benchmark 2000 0.05 8

echo ""
echo "=========================================="
echo "GENERATING PLOTS"
echo "=========================================="
echo ""

# Generate plots
python3 plot_results.py

echo ""
echo "=========================================="
echo "COMPLETE!"
echo "=========================================="
echo ""
echo "Generated files:"
echo "  • results.csv              - Raw data"
echo "  • performance_comparison.png"
echo "  • speedup_comparison.png"
echo "  • time_comparison.png"
echo "  • roofline_analysis.png"
echo "  • summary_all.png          - Combined view ⭐"
echo ""
echo "View plots:"
if command -v xdg-open &> /dev/null; then
    echo "  xdg-open summary_all.png"
elif command -v open &> /dev/null; then
    echo "  open summary_all.png"
else
    echo "  (use your image viewer)"
fi
echo ""
echo "=========================================="
echo "MODULAR DESIGN:"
echo "  • Each method in separate file"
echo "  • Clean code organization"
echo "  • Easy to modify/extend"
echo "  • Production-ready structure"
echo "=========================================="
echo ""
