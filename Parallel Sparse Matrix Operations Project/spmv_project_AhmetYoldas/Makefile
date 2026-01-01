# Modular SpMV Benchmark - Makefile
# Each method in separate files

CC = gcc
CFLAGS = -O3 -march=native -fopenmp -Wall -Wextra
LDFLAGS = -lm -fopenmp

TARGET = benchmark

# All source files
SRCS = common.c \
       csr_serial.c \
       csr_parallel.c \
       bcsr_parallel.c \
       bucket_parallel.c \
       bcsr_bucket_parallel.c \
       benchmark.c

# Object files
OBJS = $(SRCS:.c=.o)

# Headers
HEADERS = common.h \
          csr_serial.h \
          csr_parallel.h \
          bcsr_parallel.h \
          bucket_parallel.h \
          bcsr_bucket_parallel.h

all: $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "✓ Build successful!"
	@echo "=========================================="
	@echo ""
	@echo "Modular structure:"
	@echo "  • common.c/h           - Shared utilities"
	@echo "  • csr_serial.c/h       - Method 1 (baseline)"
	@echo "  • csr_parallel.c/h     - Method 2"
	@echo "  • bcsr_parallel.c/h    - Method 3"
	@echo "  • bucket_parallel.c/h  - Method 4"
	@echo "  • bcsr_bucket_parallel.c/h - Method 5 (hybrid)"
	@echo "  • benchmark.c          - Main program"
	@echo ""
	@echo "Run complete analysis:"
	@echo "  ./run_all.sh"
	@echo ""

$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS) results.csv *.png

plots: $(TARGET)
	@echo "Running benchmark and generating plots..."
	./$(TARGET) 2000 0.05 8
	python3 plot_results.py

help:
	@echo "Modular SpMV Benchmark"
	@echo "======================"
	@echo ""
	@echo "Structure:"
	@echo "  5 methods in separate files"
	@echo "  1 Serial + 4 Parallel"
	@echo ""
	@echo "Targets:"
	@echo "  make        - Build benchmark"
	@echo "  make plots  - Run benchmark + plots"
	@echo "  make clean  - Remove generated files"
	@echo ""
	@echo "Complete workflow:"
	@echo "  ./run_all.sh  - Automated (recommended!)"

.PHONY: all clean plots help
