#!/bin/bash

echo "=== MPI Performance Graph Generator ==="
echo ""

# Check if Python3 is installed
if ! command -v python3 &> /dev/null; then
    echo "Error: Python3 is not installed!"
    echo "Please install Python3:"
    echo "  sudo apt-get install python3"
    exit 1
fi

# Check if matplotlib is installed
python3 -c "import matplotlib" 2>/dev/null
if [ $? -ne 0 ]; then
    echo "matplotlib not found. Installing..."
    pip3 install matplotlib --user
    
    # Check again
    python3 -c "import matplotlib" 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "Error: Could not install matplotlib!"
        echo "Please install manually:"
        echo "  pip3 install matplotlib"
        echo "  # OR"
        echo "  sudo apt-get install python3-matplotlib"
        exit 1
    fi
fi

# Check if CSV files exist
if [ ! -f "results.csv" ]; then
    echo "Error: results.csv not found!"
    echo "Please run benchmark first:"
    echo "  ./benchmark.sh"
    echo "  ./analyze_results.sh"
    exit 1
fi

if [ ! -f "analysis.csv" ]; then
    echo "Error: analysis.csv not found!"
    echo "Please run analyze_results.sh first."
    exit 1
fi

# Run the Python script
echo "Generating performance graphs..."
echo ""

python3 plot_results.py

if [ $? -eq 0 ]; then
    echo ""
    echo "=== Success! ==="
    echo ""
    echo "View your graphs:"
    echo "  display execution_times.png"
    echo "  display speedup.png"
    echo "  display efficiency.png"
    echo "  display combined_performance.png"
    echo "  display summary.png"
    echo ""
    echo "Or open with:"
    echo "  xdg-open *.png"
else
    echo ""
    echo "Error: Graph generation failed!"
    exit 1
fi