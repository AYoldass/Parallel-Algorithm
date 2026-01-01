#!/bin/bash

# Force English locale for numeric operations
export LC_ALL=C
export LC_NUMERIC=C

# CSV dosyasını oku ve analiz yap
if [ ! -f "results.csv" ]; then
    echo "Error: results.csv not found!"
    echo "Please run benchmark.sh first."
    exit 1
fi

echo "=== Performance Analysis ==="
echo ""

# CSV'den verileri oku (header'ı atla)
tail -n +2 results.csv > temp_results.txt

# Sequential time'ı al (ilk satır)
SEQ_TIME=$(head -n 1 temp_results.txt | cut -d',' -f5)

echo "Sequential Time: $SEQ_TIME seconds"
echo ""

# Speedup ve Efficiency tablosu oluştur
echo "Process,Average_Time,Speedup,Efficiency" > analysis.csv

# Her paralel versiyon için hesapla
while IFS=',' read -r procs run1 run2 run3 avg; do
    if [ "$procs" != "Sequential" ] && [ -n "$procs" ]; then
        # Speedup = T_sequential / T_parallel
        SPEEDUP=$(echo "scale=4; $SEQ_TIME / $avg" | bc -l)
        
        # Efficiency = Speedup / Number_of_processes
        EFFICIENCY=$(echo "scale=4; $SPEEDUP / $procs" | bc -l)
        
        # Efficiency percentage
        EFF_PERCENT=$(echo "scale=2; $EFFICIENCY * 100" | bc -l)
        
        # CSV'ye yaz
        echo "$procs,$avg,$SPEEDUP,$EFFICIENCY" >> analysis.csv
        
        # Ekrana yazdır (locale-safe)
        echo "Processes: $procs"
        echo "  Average Time: $avg seconds"
        echo "  Speedup: $SPEEDUP"
        echo "  Efficiency: $EFFICIENCY ($EFF_PERCENT%)"
        echo ""
    fi
done < temp_results.txt

# Temizlik
rm temp_results.txt

echo "Analysis saved to analysis.csv"
echo ""
echo "=== Summary Tables ==="
echo ""

echo "Table 1: Execution Times"
echo "------------------------"
column -t -s',' results.csv 2>/dev/null || cat results.csv
echo ""

echo "Table 2: Speedup and Efficiency"
echo "--------------------------------"
column -t -s',' analysis.csv 2>/dev/null || cat analysis.csv
echo ""

# LaTeX formatında tablolar oluştur (rapor için)
echo "=== LaTeX Format (for report) ==="
echo ""

echo "% Table 1: Execution Times"
echo "\\begin{table}[h]"
echo "\\centering"
echo "\\begin{tabular}{|c|c|c|c|c|}"
echo "\\hline"
echo "Process & Run 1 (s) & Run 2 (s) & Run 3 (s) & Average (s) \\\\\\"
echo "\\hline"
tail -n +2 results.csv | while IFS=',' read -r procs run1 run2 run3 avg; do
    if [ -n "$procs" ]; then
        echo "$procs & $run1 & $run2 & $run3 & $avg \\\\\\"
        echo "\\hline"
    fi
done
echo "\\end{tabular}"
echo "\\caption{Execution times for different process counts}"
echo "\\end{table}"
echo ""

echo "% Table 2: Speedup and Efficiency"
echo "\\begin{table}[h]"
echo "\\centering"
echo "\\begin{tabular}{|c|c|c|c|}"
echo "\\hline"
echo "Processes & Time (s) & Speedup & Efficiency \\\\\\"
echo "\\hline"
tail -n +2 analysis.csv | while IFS=',' read -r procs avg speedup efficiency; do
    if [ -n "$procs" ]; then
        echo "$procs & $avg & $speedup & $efficiency \\\\\\"
        echo "\\hline"
    fi
done
echo "\\end{tabular}"
echo "\\caption{Speedup and efficiency analysis}"
echo "\\end{table}"
echo ""

# Markdown formatı da ekleyelim
echo "=== Markdown Format (for README) ==="
echo ""
echo "## Execution Times"
echo ""
echo "| Process | Run 1 | Run 2 | Run 3 | Average |"
echo "|---------|-------|-------|-------|---------|"
tail -n +2 results.csv | while IFS=',' read -r procs run1 run2 run3 avg; do
    if [ -n "$procs" ]; then
        echo "| $procs | $run1 | $run2 | $run3 | $avg |"
    fi
done
echo ""
echo "## Speedup and Efficiency"
echo ""
echo "| Processes | Time (s) | Speedup | Efficiency |"
echo "|-----------|----------|---------|------------|"
tail -n +2 analysis.csv | while IFS=',' read -r procs avg speedup efficiency; do
    if [ -n "$procs" ]; then
        eff_percent=$(echo "scale=1; $efficiency * 100" | bc -l)
        echo "| $procs | $avg | $speedup | $efficiency (${eff_percent}%) |"
    fi
done