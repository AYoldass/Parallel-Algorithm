#!/bin/bash

# Force English locale
export LC_ALL=C
export LC_NUMERIC=C

echo "=== MPI vs OpenMP Performance Comparison ==="
echo ""

# Check if both result files exist
if [ ! -f "results.csv" ]; then
    echo "Error: results.csv (MPI) not found!"
    echo "Please run: ./benchmark.sh"
    exit 1
fi

if [ ! -f "results_openmp.csv" ]; then
    echo "Error: results_openmp.csv not found!"
    echo "Please run: ./benchmark_openmp.sh"
    exit 1
fi

# Get sequential time (should be same for both)
SEQ_TIME_MPI=$(tail -n +2 results.csv | head -n 1 | cut -d',' -f5)
SEQ_TIME_OMP=$(tail -n +2 results_openmp.csv | head -n 1 | cut -d',' -f5)

echo "Sequential Time (MPI benchmark): $SEQ_TIME_MPI seconds"
echo "Sequential Time (OMP benchmark): $SEQ_TIME_OMP seconds"
echo ""

# Create comparison table
echo "Parallelism,Workers,Time,Speedup,Efficiency" > comparison.csv

# Extract MPI results
tail -n +2 analysis.csv | while IFS=',' read -r procs avg speedup efficiency; do
    echo "MPI,$procs,$avg,$speedup,$efficiency" >> comparison.csv
done

# Extract OpenMP results
tail -n +2 analysis_openmp.csv | while IFS=',' read -r threads avg speedup efficiency; do
    echo "OpenMP,$threads,$avg,$speedup,$efficiency" >> comparison.csv
done

echo "=== Comparison Table ==="
echo ""
echo "Method    Workers  Time(s)     Speedup  Efficiency"
echo "---------------------------------------------------"
column -t -s',' comparison.csv

echo ""
echo "=== LaTeX Comparison Table ==="
echo ""

echo "\\begin{table}[h]"
echo "\\centering"
echo "\\caption{MPI vs OpenMP performance comparison}"
echo "\\label{tab:comparison}"
echo "\\begin{tabular}{|c|c|c|c|c|}"
echo "\\hline"
echo "\\textbf{Method} & \\textbf{Workers} & \\textbf{Time (s)} & \\textbf{Speedup} & \\textbf{Efficiency} \\\\\\\\"
echo "\\hline"

# MPI results
tail -n +2 analysis.csv | while IFS=',' read -r procs avg speedup efficiency; do
    eff_percent=$(echo "scale=1; $efficiency * 100" | bc -l)
    echo "MPI & $procs & $avg & $speedup & $efficiency (${eff_percent}\\%) \\\\\\\\"
done
echo "\\hline"

# OpenMP results
tail -n +2 analysis_openmp.csv | while IFS=',' read -r threads avg speedup efficiency; do
    eff_percent=$(echo "scale=1; $efficiency * 100" | bc -l)
    echo "OpenMP & $threads & $avg & $speedup & $efficiency (${eff_percent}\\%) \\\\\\\\"
done
echo "\\hline"

echo "\\end{tabular}"
echo "\\end{table}"
echo ""

echo "=== Analysis ==="
echo ""

# Find best performer
BEST_MPI=$(tail -n +2 analysis.csv | sort -t',' -k3 -rn | head -n1)
BEST_OMP=$(tail -n +2 analysis_openmp.csv | sort -t',' -k3 -rn | head -n1)

MPI_PROCS=$(echo $BEST_MPI | cut -d',' -f1)
MPI_SPEEDUP=$(echo $BEST_MPI | cut -d',' -f3)
OMP_THREADS=$(echo $BEST_OMP | cut -d',' -f1)
OMP_SPEEDUP=$(echo $BEST_OMP | cut -d',' -f3)

echo "Best MPI Performance:"
echo "  Processes: $MPI_PROCS"
echo "  Speedup: $MPI_SPEEDUP"
echo ""

echo "Best OpenMP Performance:"
echo "  Threads: $OMP_THREADS"
echo "  Speedup: $OMP_SPEEDUP"
echo ""

# Compare
COMPARISON=$(echo "$OMP_SPEEDUP > $MPI_SPEEDUP" | bc -l)
if [ "$COMPARISON" = "1" ]; then
    DIFF=$(echo "scale=4; $OMP_SPEEDUP / $MPI_SPEEDUP" | bc -l)
    echo "Winner: OpenMP (${DIFF}x better speedup)"
else
    DIFF=$(echo "scale=4; $MPI_SPEEDUP / $OMP_SPEEDUP" | bc -l)
    echo "Winner: MPI (${DIFF}x better speedup)"
fi

echo ""
echo "Comparison data saved to: comparison.csv"