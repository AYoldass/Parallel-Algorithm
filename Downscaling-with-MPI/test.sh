#!/bin/bash

echo "=== Image Downscaling Performance Test ==="
echo ""

# Otomatik olarak bir jpg dosyası bul
if [ -f "aybu.jpg" ]; then
    IMAGE="aybu.jpg"
elif [ -f "input.jpg" ]; then
    IMAGE="input.jpg"
else
    # Dizindeki ilk jpg dosyasını bul
    IMAGE=$(ls *.jpg 2>/dev/null | head -n 1)
    if [ -z "$IMAGE" ]; then
        echo "Error: No .jpg image found in current directory!"
        echo "Please provide an input image (e.g., aybu.jpg or input.jpg)"
        exit 1
    fi
fi

echo "Using image: $IMAGE"
echo ""

# Test 1: Sequential
echo "Running Sequential Version..."
./seq_main $IMAGE aybu_seq.jpg

if [ $? -ne 0 ]; then
    echo "Error: Sequential version failed!"
    exit 1
fi

echo ""
echo "Running Parallel Versions..."
echo ""

# Test 2: 1 process
echo "1 Process:"
mpirun -np 1 ./mpi_main $IMAGE aybu_mpi_1.jpg
echo ""

# Test 3: 2 processes
echo "2 Processes:"
mpirun -np 2 ./mpi_main $IMAGE aybu_mpi_2.jpg
echo ""

# Test 4: 4 processes
echo "4 Processes:"
mpirun -np 4 ./mpi_main $IMAGE aybu_mpi_4.jpg
echo ""

# Test 5: 8 processes
echo "8 Processes:"
mpirun -np 8 ./mpi_main $IMAGE aybu_mpi_8.jpg
echo ""

echo "=== Tests Complete ==="
echo ""
echo "Output files created:"
ls -lh output_*.jpg 2>/dev/null