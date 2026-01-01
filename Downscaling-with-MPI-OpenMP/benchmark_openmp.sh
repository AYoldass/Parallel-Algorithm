#!/bin/bash

# Force English locale for numeric operations
export LC_ALL=C
export LC_NUMERIC=C

# Auto-detect a jpg file
if [ -f "aybu.jpg" ]; then
    IMAGE="aybu.jpg"
elif [ -f "input.jpg" ]; then
    IMAGE="input.jpg"
else
    IMAGE=$(ls *.jpg 2>/dev/null | head -n 1)
    if [ -z "$IMAGE" ]; then
        echo "Error: No .jpg file found in current directory!"
        echo "Please provide an input image (e.g., aybu.jpg or input.jpg)"
        exit 1
    fi
fi

RUNS=3

echo "=== OpenMP Image Downscaling Benchmark ==="
echo "Image: $IMAGE"
echo "Number of runs per test: $RUNS"
echo ""

# Create CSV header
echo "Threads,Run1,Run2,Run3,Average" > results_openmp.csv

# Function to calculate average
calculate_average() {
    local sum=0
    local count=0
    for val in "$@"; do
        sum=$(echo "$sum + $val" | bc -l)
        count=$((count + 1))
    done
    # Add leading zero if needed
    local avg=$(echo "scale=6; $sum / $count" | bc -l)
    if [[ $avg == .* ]]; then
        avg="0$avg"
    fi
    echo "$avg"
}

# Sequential version (for comparison)
echo "Running Sequential version..."
echo -n "Sequential," >> results_openmp.csv
TIMES=()
for i in $(seq 1 $RUNS); do
    echo "  Run $i/$RUNS..."
    TIME=$(./seq_main $IMAGE aybu_seq.jpg 2>/dev/null | grep "Elapsed time:" | awk '{print $3}')
    if [[ $TIME == .* ]]; then
        TIME="0$TIME"
    fi
    TIMES+=($TIME)
    echo -n "$TIME," >> results_openmp.csv
done
AVG=$(calculate_average "${TIMES[@]}")
echo "$AVG" >> results_openmp.csv
echo "  Average: $AVG seconds"
echo ""

# OpenMP versions with different thread counts
for NTHREADS in 1 2 4 8; do
    echo "Running with $NTHREADS thread(s)..."
    echo -n "$NTHREADS," >> results_openmp.csv
    TIMES=()
    for i in $(seq 1 $RUNS); do
        echo "  Run $i/$RUNS..."
        TIME=$(./openmp_main $IMAGE aybu_omp_${NTHREADS}.jpg $NTHREADS 2>/dev/null | grep "Elapsed time:" | awk '{print $3}')
        if [[ $TIME == .* ]]; then
            TIME="0$TIME"
        fi
        TIMES+=($TIME)
        echo -n "$TIME," >> results_openmp.csv
    done
    AVG=$(calculate_average "${TIMES[@]}")
    echo "$AVG" >> results_openmp.csv
    echo "  Average: $AVG seconds"
    echo ""
done

echo "=== OpenMP Benchmark Complete ==="
echo "Results saved to results_openmp.csv"
echo ""
echo "CSV Contents:"
cat results_openmp.csv
echo ""
echo "Run for analysis: ./analyze_openmp.sh"