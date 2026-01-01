# Compiler settings
CC = gcc
MPICC = mpicc
CFLAGS = -Wall -O3 -lm
LDFLAGS = -lm
OMPFLAGS = -fopenmp

# Targets
SEQ_TARGET = seq_main
MPI_TARGET = mpi_main
OMP_TARGET = openmp_main

# Source files
SEQ_SRC = seq_main.c
MPI_SRC = mpi_main.c
OMP_SRC = openmp_main.c

# Header files
HEADERS = stb_image.h stb_image_write.h

.PHONY: all clean test test_seq test_mpi test_omp benchmark benchmark_tr benchmark_en benchmark_omp plot help yardim

# Default target
all: $(SEQ_TARGET) $(MPI_TARGET) $(OMP_TARGET)
	@echo "Build complete!"
	@echo "Run 'make test' to test all programs"
	@echo "Run 'make benchmark_omp' for OpenMP benchmarks"
	@echo "Run 'make benchmark' for MPI benchmarks"
	@echo "Run 'make plot' to generate performance graphs"
	@echo "Run 'make help' for more options"

# Sequential version
$(SEQ_TARGET): $(SEQ_SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SEQ_SRC) -o $(SEQ_TARGET) $(LDFLAGS)
	@echo "Sequential version compiled: $(SEQ_TARGET)"

# Parallel MPI version
$(MPI_TARGET): $(MPI_SRC) $(HEADERS)
	$(MPICC) $(CFLAGS) $(MPI_SRC) -o $(MPI_TARGET) $(LDFLAGS)
	@echo "Parallel MPI version compiled: $(MPI_TARGET)"

# OpenMP version
$(OMP_TARGET): $(OMP_SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(OMPFLAGS) $(OMP_SRC) -o $(OMP_TARGET) $(LDFLAGS)
	@echo "OpenMP version compiled: $(OMP_TARGET)"

# Run basic tests
test: all
	@echo "Running basic tests..."
	@chmod +x test.sh
	@./test.sh

# Test only sequential version
test_seq: $(SEQ_TARGET)
	@echo "=== Testing Sequential Version ==="
	@if [ ! -f "aybu.jpg" ]; then \
		echo "Error: aybu.jpg not found!"; \
		echo "Please provide an input image named 'aybu.jpg'"; \
		exit 1; \
	fi
	@echo "Running sequential version..."
	@./$(SEQ_TARGET) aybu.jpg aybu_seq.jpg
	@echo "Output saved to: aybu_seq.jpg"
	@echo "Sequential test complete!"

# Test only OpenMP version
test_omp: $(OMP_TARGET)
	@echo "=== Testing OpenMP Version ==="
	@if [ ! -f "aybu.jpg" ]; then \
		echo "Error: aybu.jpg not found!"; \
		echo "Please provide an input image named 'aybu.jpg'"; \
		exit 1; \
	fi
	@echo "Testing with 1 thread..."
	@./$(OMP_TARGET) aybu.jpg aybu_omp_1.jpg 1
	@echo ""
	@echo "Testing with 2 threads..."
	@./$(OMP_TARGET) aybu.jpg aybu_omp_2.jpg 2
	@echo ""
	@echo "Testing with 4 threads..."
	@./$(OMP_TARGET) aybu.jpg aybu_omp_4.jpg 4
	@echo ""
	@echo "All OpenMP tests complete!"
	@echo "Outputs saved: aybu_omp_1.jpg, aybu_omp_2.jpg, aybu_omp_4.jpg"
	@echo "=== Testing Parallel MPI Version ==="
	@if [ ! -f "aybu.jpg" ]; then \
		echo "Error: aybu.jpg not found!"; \
		echo "Please provide an input image named 'aybu.jpg'"; \
		exit 1; \
	fi
	@echo "Testing with 1 process..."
	@mpirun -np 1 ./$(MPI_TARGET) aybu.jpg aybu_mpi_1.jpg
	@echo ""
	@echo "Testing with 2 processes..."
	@mpirun -np 2 ./$(MPI_TARGET) aybu.jpg aybu_mpi_2.jpg
	@echo ""
	@echo "Testing with 4 processes..."
	@mpirun -np 4 ./$(MPI_TARGET) aybu.jpg aybu_mpi_4.jpg
	@echo ""
	@echo "All MPI tests complete!"
	@echo "Outputs saved: aybu_mpi_1.jpg, aybu_mpi_2.jpg, aybu_mpi_4.jpg"

# Run benchmarks (English)
benchmark: all
	@echo "Running benchmarks..."
	@chmod +x benchmark.sh
	@./benchmark.sh
	@echo ""
	@echo "Analyzing results..."
	@chmod +x analyze_results.sh
	@./analyze_results.sh

# English benchmark (alias)
benchmark_en: benchmark

# Turkish benchmark
benchmark_tr: all
	@echo "Benchmark testleri çalıştırılıyor..."
	@chmod +x benchmark_tr.sh
	@./benchmark_tr.sh
	@echo ""
	@echo "Sonuçlar analiz ediliyor..."
	@chmod +x analyze_results_tr.sh
	@./analyze_results_tr.sh

# OpenMP benchmark
benchmark_omp: all
	@echo "Running OpenMP benchmarks..."
	@chmod +x benchmark_openmp.sh
	@./benchmark_openmp.sh
	@echo ""
	@echo "Analyzing OpenMP results..."
	@chmod +x analyze_openmp.sh
	@./analyze_openmp.sh

# Generate performance graphs
plot:
	@echo "Generating performance graphs..."
	@chmod +x plot_graphs.sh plot_results.py
	@./plot_graphs.sh

# Clean build artifacts
clean:
	rm -f $(SEQ_TARGET) $(MPI_TARGET) $(OMP_TARGET)
	rm -f output_*.jpg aybu_seq.jpg aybu_mpi_*.jpg aybu_omp_*.jpg
	rm -f results.csv analysis.csv results_openmp.csv analysis_openmp.csv
	rm -f *.png
	@echo "Cleaned all build artifacts and output files"

# Help target
help:
	@echo "Available targets:"
	@echo "  make all           - Build all versions (seq, MPI, OpenMP)"
	@echo "  make seq_main      - Build only sequential version"
	@echo "  make mpi_main      - Build only MPI version"
	@echo "  make openmp_main   - Build only OpenMP version"
	@echo ""
	@echo "Testing:"
	@echo "  make test          - Run all tests"
	@echo "  make test_seq      - Test sequential version"
	@echo "  make test_mpi      - Test MPI version"
	@echo "  make test_omp      - Test OpenMP version"
	@echo ""
	@echo "Benchmarking:"
	@echo "  make benchmark     - Run MPI benchmarks (English)"
	@echo "  make benchmark_en  - Run MPI benchmarks (English, alias)"
	@echo "  make benchmark_tr  - Run MPI benchmarks (Turkish)"
	@echo "  make benchmark_omp - Run OpenMP benchmarks"
	@echo ""
	@echo "Analysis & Visualization:"
	@echo "  make plot          - Generate performance graphs"
	@echo ""
	@echo "Utilities:"
	@echo "  make clean         - Remove all build artifacts"
	@echo "  make help          - Show this help"
	@echo "  make yardim        - Turkish help"
	@echo ""
	@echo "Complete workflow:"
	@echo "  make all                      # Compile everything"
	@echo "  ./benchmark.sh                # MPI benchmark"
	@echo "  ./analyze_results.sh          # MPI analysis"
	@echo "  ./benchmark_openmp.sh         # OpenMP benchmark"
	@echo "  ./analyze_openmp.sh           # OpenMP analysis"
	@echo "  ./compare_results.sh          # Compare MPI vs OpenMP"
	@echo "  python3 plot_comparison.py    # Generate comparison graphs"

# Türkçe yardım
yardim:
	@echo "Kullanılabilir komutlar:"
	@echo "  make all           - Tüm versiyonları derle (sıralı, MPI, OpenMP)"
	@echo "  make seq_main      - Sadece sıralı versiyonu derle"
	@echo "  make mpi_main      - Sadece MPI versiyonu derle"
	@echo "  make openmp_main   - Sadece OpenMP versiyonu derle"
	@echo ""
	@echo "Test:"
	@echo "  make test          - Tüm testleri çalıştır"
	@echo "  make test_seq      - Sıralı versiyonu test et"
	@echo "  make test_mpi      - MPI versiyonunu test et"
	@echo "  make test_omp      - OpenMP versiyonunu test et"
	@echo ""
	@echo "Benchmark:"
	@echo "  make benchmark     - MPI benchmark (İngilizce)"
	@echo "  make benchmark_tr  - MPI benchmark (Türkçe)"
	@echo "  make benchmark_omp - OpenMP benchmark"
	@echo ""
	@echo "Analiz & Görselleştirme:"
	@echo "  make plot          - Performans grafiklerini oluştur"
	@echo ""
	@echo "Araçlar:"
	@echo "  make clean         - Tüm derleme dosyalarını temizle"
	@echo "  make help          - İngilizce yardım"
	@echo "  make yardim        - Bu yardım mesajı"
	@echo ""
	@echo "Tam iş akışı:"
	@echo "  make all                      # Herşeyi derle"
	@echo "  ./benchmark_tr.sh             # MPI benchmark"
	@echo "  ./analyze_results_tr.sh       # MPI analiz"
	@echo "  ./benchmark_openmp.sh         # OpenMP benchmark"
	@echo "  ./analyze_openmp.sh           # OpenMP analiz"
	@echo "  ./compare_results.sh          # MPI vs OpenMP karşılaştır"
	@echo "  python3 plot_comparison.py    # Karşılaştırma grafikleri"