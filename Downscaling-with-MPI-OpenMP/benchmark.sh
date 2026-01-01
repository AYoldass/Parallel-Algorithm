#!/bin/bash

IMAGE="aybu.jpg"
RUNS=3

echo "=== Image Downscaling Benchmark ==="
echo "Image: $IMAGE"
echo "Runs per test: $RUNS"
echo ""

# Create CSV header
echo "Process,Run1,Run2,Run3,Average" > results.csv

# Function to calculate average
calculate_average() {
    local sum=0
    local count=0
    for val in "$@"; do
        sum=$(echo "$sum + $val" | bc -l)
        count=$((count + 1))
    done
    echo "scale=6; $sum / $count" | bc -l
}

# Sequential version
echo "Running Sequential version..."
echo -n "Sequential," >> results.csv
TIMES=()
for i in $(seq 1 $RUNS); do
    echo "  Run $i/3..."
    TIME=$(./seq_main $IMAGE aybu_seq.jpg 2>/dev/null | grep "Elapsed time:" | awk '{print $3}')
    TIMES+=($TIME)
    echo -n "$TIME," >> results.csv
done
AVG=$(calculate_average "${TIMES[@]}")
echo "$AVG" >> results.csv
echo "  Average: $AVG seconds"
echo ""

# Parallel versions with different process counts
for NPROCS in 1 2 4 8; do
    echo "Running with $NPROCS process(es)..."
    echo -n "$NPROCS," >> results.csv
    TIMES=()
    for i in $(seq 1 $RUNS); do
        echo "  Run $i/3..."
        TIME=$(mpirun -np $NPROCS ./mpi_main $IMAGE aybu_mpi_${NPROCS}.jpg 2>/dev/null | grep "Elapsed time:" | awk '{print $3}')
        TIMES+=($TIME)
        echo -n "$TIME," >> results.csv
    done
    AVG=$(calculate_average "${TIMES[@]}")
    echo "$AVG" >> results.csv
    echo "  Average: $AVG seconds"
    echo ""
done

echo "=== Benchmark Complete ==="
echo "Results saved to results.csv"
echo ""
echo "CSV Contents:"
cat results.csv