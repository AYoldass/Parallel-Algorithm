#!/bin/bash

# Force English locale for numeric operations
export LC_ALL=C
export LC_NUMERIC=C

if [ ! -f "results_openmp.csv" ]; then
    echo "Error: results_openmp.csv not found!"
    echo "Please run benchmark_openmp.sh first."
    exit 1
fi

echo "=== OpenMP Performance Analysis ==="
echo ""

tail -n +2 results_openmp.csv > temp_results_omp.txt
SEQ_TIME=$(head -n 1 temp_results_omp.txt | cut -d',' -f5)

echo "Sequential Time: $SEQ_TIME seconds"
echo ""

echo "Threads,Average_Time,Speedup,Efficiency" > analysis_openmp.csv

while IFS=',' read -r threads run1 run2 run3 avg; do
    if [ "$threads" != "Sequential" ] && [ -n "$threads" ]; then
        SPEEDUP=$(echo "scale=4; $SEQ_TIME / $avg" | bc -l)
        EFFICIENCY=$(echo "scale=4; $SPEEDUP / $threads" | bc -l)
        EFF_PERCENT=$(echo "scale=2; $EFFICIENCY * 100" | bc -l)
        
        echo "$threads,$avg,$SPEEDUP,$EFFICIENCY" >> analysis_openmp.csv
        
        echo "Number of Threads: $threads"
        echo "  Average Time: $avg seconds"
        echo "  Speedup: $SPEEDUP"
        echo "  Efficiency: $EFFICIENCY ($EFF_PERCENT%)"
        echo ""
    fi
done < temp_results_omp.txt

rm temp_results_omp.txt

echo "Analysis results saved to analysis_openmp.csv"
echo ""
echo "=== Summary Tables ==="
echo ""

echo "Table 1: Execution Times (OpenMP)"
echo "-----------------------------------"
column -t -s',' results_openmp.csv 2>/dev/null || cat results_openmp.csv
echo ""

echo "Table 2: Speedup and Efficiency (OpenMP)"
echo "-----------------------------------------"
column -t -s',' analysis_openmp.csv 2>/dev/null || cat analysis_openmp.csv
echo ""

echo "=== LaTeX Format (for Report) ==="
echo ""

echo "% Table: OpenMP Execution Times"
echo "\\begin{table}[h]"
echo "\\centering"
echo "\\caption{Execution times with OpenMP (seconds)}"
echo "\\label{tab:omp_execution}"
echo "\\begin{tabular}{|c|c|c|c|c|}"
echo "\\hline"
echo "\\textbf{Threads} & \\textbf{Run 1} & \\textbf{Run 2} & \\textbf{Run 3} & \\textbf{Average} \\\\\\\\"
echo "\\hline"
tail -n +2 results_openmp.csv | while IFS=',' read -r threads run1 run2 run3 avg; do
    if [ -n "$threads" ]; then
        echo "$threads & $run1 & $run2 & $run3 & $avg \\\\\\\\"
        echo "\\hline"
    fi
done
echo "\\end{tabular}"
echo "\\end{table}"
echo ""

echo "% Table: OpenMP Speedup and Efficiency"
echo "\\begin{table}[h]"
echo "\\centering"
echo "\\caption{OpenMP speedup and efficiency analysis}"
echo "\\label{tab:omp_speedup}"
echo "\\begin{tabular}{|c|c|c|c|}"
echo "\\hline"
echo "\\textbf{Threads} & \\textbf{Time (s)} & \\textbf{Speedup} & \\textbf{Efficiency} \\\\\\\\"
echo "\\hline"
tail -n +2 analysis_openmp.csv | while IFS=',' read -r threads avg speedup efficiency; do
    if [ -n "$threads" ]; then
        eff_percent=$(echo "scale=1; $efficiency * 100" | bc -l)
        echo "$threads & $avg & $speedup & $efficiency (${eff_percent}\\%) \\\\\\\\"
        echo "\\hline"
    fi
done
echo "\\end{tabular}"
echo "\\end{table}"
echo ""

echo "=== Performance Summary ==="
echo ""
echo "Sequential time: $SEQ_TIME seconds"
echo ""
echo "Best speedup:"
BEST_SPEEDUP=$(tail -n +2 analysis_openmp.csv | cut -d',' -f3 | sort -rn | head -n1)
echo "  Speedup: $BEST_SPEEDUP"
echo ""
echo "Average efficiency:"
AVG_EFF=$(tail -n +2 analysis_openmp.csv | cut -d',' -f4 | awk '{sum+=$1; count++} END {printf "%.4f", sum/count}')
AVG_EFF_PERCENT=$(echo "scale=2; $AVG_EFF * 100" | bc -l)
echo "  Efficiency: $AVG_EFF ($AVG_EFF_PERCENT%)"