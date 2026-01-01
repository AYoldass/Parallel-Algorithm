/**
 * Complete SpMV Benchmark - Modular Version
 * Tests 5 methods: 1 Serial + 4 Parallel
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>

#include "common.h"
#include "csr_serial.h"
#include "csr_parallel.h"
#include "bcsr_parallel.h"
#include "bucket_parallel.h"
#include "bcsr_bucket_parallel.h"

// Timing
static inline double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
}

// GFlop/s
static inline double compute_gflops(int nnz, double time) {
    return (2.0 * nnz) / time / 1e9;
}

// Verification
int verify(const double *y1, const double *y2, int n) {
    double max_diff = 0.0;
    for (int i = 0; i < n; i++) {
        double diff = fabs(y1[i] - y2[i]);
        if (diff > max_diff) max_diff = diff;
    }
    return max_diff < 1e-10;
}

int main(int argc, char **argv) {
    // Parse arguments
    int n = (argc > 1) ? atoi(argv[1]) : 2000;
    double density = (argc > 2) ? atof(argv[2]) : 0.05;
    int threads = (argc > 3) ? atoi(argv[3]) : 8;
    
    omp_set_num_threads(threads);
    
    printf("========================================\n");
    printf("MODULAR SpMV BENCHMARK\n");
    printf("1 Serial + 4 Parallel Methods\n");
    printf("========================================\n");
    printf("Matrix size: %d × %d\n", n, n);
    printf("Density: %.2f%%\n", density * 100);
    printf("Threads: %d\n", threads);
    printf("========================================\n\n");
    
    // Generate CSR matrix
    printf("Generating sparse matrix...\n");
    srand(42);
    CSR_Matrix *A_csr = csr_random(n, density);
    printf("  Actual nnz: %d\n", A_csr->nnz);
    printf("  Actual density: %.2f%%\n\n", 100.0 * A_csr->nnz / (n * n));
    
    // Convert to BCSR
    printf("Converting CSR → BCSR (4×4)...\n");
    BCSR_Matrix *A_bcsr = csr_to_bcsr(A_csr);
    printf("  Block rows: %d\n", A_bcsr->block_rows);
    printf("  Number of blocks: %d\n", A_bcsr->num_blocks);
    printf("  BCSR storage overhead: %.1f×\n\n", 
           (double)(A_bcsr->num_blocks * 16) / A_csr->nnz);
    
    // Allocate vectors
    double *x = (double*)malloc(n * sizeof(double));
    double *y1 = (double*)malloc(n * sizeof(double));
    double *y2 = (double*)malloc(n * sizeof(double));
    double *y3 = (double*)malloc(n * sizeof(double));
    double *y4 = (double*)malloc(n * sizeof(double));
    double *y5 = (double*)malloc(n * sizeof(double));
    
    for (int i = 0; i < n; i++) {
        x[i] = (double)rand() / RAND_MAX;
    }
    
    printf("========================================\n");
    printf("RUNNING BENCHMARKS (5 METHODS)\n");
    printf("========================================\n\n");
    
    // Results storage
    const char *method_names[5] = {
        "CSR Serial",
        "CSR Parallel",
        "BCSR Parallel",
        "CSR+Bucket Parallel (Optimized)",
        "BCSR+Bucket Parallel (Optimized)"
    };
    double times[5];
    double gflops[5];
    double speedups[5];
    int correctness[5];
    
    // ===== METHOD 1: CSR Serial (BASELINE) =====
    printf("1. CSR SERIAL (Baseline)\n");
    printf("   File: csr_serial.c\n");
    spmv_csr_serial(A_csr, x, y1);
    double t1 = get_time();
    spmv_csr_serial(A_csr, x, y1);
    times[0] = get_time() - t1;
    gflops[0] = compute_gflops(A_csr->nnz, times[0]);
    speedups[0] = 1.0;
    correctness[0] = 1;
    printf("   Time: %.6f sec\n", times[0]);
    printf("   Performance: %.3f GFlop/s\n", gflops[0]);
    printf("   Speedup: 1.00× (baseline)\n\n");
    
    // ===== METHOD 2: CSR Parallel =====
    printf("2. CSR PARALLEL\n");
    printf("   File: csr_parallel.c\n");
    printf("   Optimization: OpenMP dynamic scheduling\n");
    spmv_csr_parallel(A_csr, x, y2);
    double t2 = get_time();
    spmv_csr_parallel(A_csr, x, y2);
    times[1] = get_time() - t2;
    gflops[1] = compute_gflops(A_csr->nnz, times[1]);
    speedups[1] = times[0] / times[1];
    correctness[1] = verify(y1, y2, n);
    printf("   Time: %.6f sec\n", times[1]);
    printf("   Performance: %.3f GFlop/s\n", gflops[1]);
    printf("   Speedup: %.2f× vs baseline\n", speedups[1]);
    printf("   Correctness: %s\n\n", correctness[1] ? "✓ PASS" : "✗ FAIL");
    
    // ===== METHOD 3: BCSR Parallel =====
    printf("3. BCSR PARALLEL\n");
    printf("   File: bcsr_parallel.c\n");
    printf("   Optimization: 4×4 blocking + OpenMP\n");
    spmv_bcsr_parallel(A_bcsr, x, y3);
    double t3 = get_time();
    spmv_bcsr_parallel(A_bcsr, x, y3);
    times[2] = get_time() - t3;
    gflops[2] = compute_gflops(A_csr->nnz, times[2]);
    speedups[2] = times[0] / times[2];
    correctness[2] = verify(y1, y3, n);
    printf("   Time: %.6f sec\n", times[2]);
    printf("   Performance: %.3f GFlop/s\n", gflops[2]);
    printf("   Speedup: %.2f× vs baseline\n", speedups[2]);
    printf("   Correctness: %s\n\n", correctness[2] ? "✓ PASS" : "✗ FAIL");
    
    // ===== METHOD 4: Bucket Parallel =====
    printf("4. CSR+BUCKET PARALLEL (OPTIMIZED)\n");
    printf("   File: bucket_parallel.c\n");
    printf("   Optimization: Adaptive buckets + OpenMP\n");
    
    // Calculate bucket info (same logic as in bucket_parallel.c)
    int num_threads = omp_get_max_threads();
    int min_buckets = num_threads * 4;
    int bucket_size_calc = A_csr->rows / min_buckets;
    if (bucket_size_calc < 32) bucket_size_calc = 32;
    if (bucket_size_calc > 512) bucket_size_calc = 512;
    int num_buckets = (A_csr->rows + bucket_size_calc - 1) / bucket_size_calc;
    printf("   Bucket size: %d rows\n", bucket_size_calc);
    printf("   Number of buckets: %d\n", num_buckets);
    
    spmv_bucket_parallel(A_csr, x, y4);
    double t4 = get_time();
    spmv_bucket_parallel(A_csr, x, y4);
    times[3] = get_time() - t4;
    gflops[3] = compute_gflops(A_csr->nnz, times[3]);
    speedups[3] = times[0] / times[3];
    correctness[3] = verify(y1, y4, n);
    printf("   Time: %.6f sec\n", times[3]);
    printf("   Performance: %.3f GFlop/s\n", gflops[3]);
    printf("   Speedup: %.2f× vs baseline\n", speedups[3]);
    printf("   Correctness: %s\n\n", correctness[3] ? "✓ PASS" : "✗ FAIL");
    
    // ===== METHOD 5: BCSR+Bucket Parallel (HYBRID) =====
    printf("5. BCSR+BUCKET PARALLEL (HYBRID - OPTIMIZED) ⭐ NEW!\n");
    printf("   File: bcsr_bucket_parallel.c\n");
    printf("   Optimization: 4×4 blocking + adaptive buckets + OpenMP\n");
    
    // Calculate bucket info for BCSR
    int bcsr_min_buckets = num_threads * 4;
    int bcsr_bucket_size = A_bcsr->block_rows / bcsr_min_buckets;
    if (bcsr_bucket_size < 8) bcsr_bucket_size = 8;
    if (bcsr_bucket_size > 128) bcsr_bucket_size = 128;
    int bcsr_num_buckets = (A_bcsr->block_rows + bcsr_bucket_size - 1) / bcsr_bucket_size;
    printf("   Bucket size: %d block rows (%d actual rows)\n", 
           bcsr_bucket_size, bcsr_bucket_size * 4);
    printf("   Number of buckets: %d\n", bcsr_num_buckets);
    
    spmv_bcsr_bucket_parallel(A_bcsr, x, y5);
    double t5 = get_time();
    spmv_bcsr_bucket_parallel(A_bcsr, x, y5);
    times[4] = get_time() - t5;
    gflops[4] = compute_gflops(A_csr->nnz, times[4]);
    speedups[4] = times[0] / times[4];
    correctness[4] = verify(y1, y5, n);
    printf("   Time: %.6f sec\n", times[4]);
    printf("   Performance: %.3f GFlop/s\n", gflops[4]);
    printf("   Speedup: %.2f× vs baseline\n", speedups[4]);
    printf("   Correctness: %s\n\n", correctness[4] ? "✓ PASS" : "✗ FAIL");
    
    // ===== SAVE TO CSV =====
    FILE *fp = fopen("results.csv", "w");
    if (fp) {
        fprintf(fp, "Method,Time(ms),GFlops,Speedup,Correctness\n");
        for (int i = 0; i < 5; i++) {
            fprintf(fp, "%s,%.6f,%.3f,%.2f,%s\n",
                    method_names[i],
                    times[i] * 1000,
                    gflops[i],
                    speedups[i],
                    correctness[i] ? "PASS" : "FAIL");
        }
        fclose(fp);
        printf("✓ Results saved to results.csv\n\n");
    }
    
    // ===== SUMMARY =====
    printf("========================================\n");
    printf("SUMMARY\n");
    printf("========================================\n\n");
    
    printf("┌───────────────────────────┬──────────┬─────────┬──────────┐\n");
    printf("│ Method                    │ Time(ms) │ GFlop/s │ Speedup  │\n");
    printf("├───────────────────────────┼──────────┼─────────┼──────────┤\n");
    for (int i = 0; i < 5; i++) {
        printf("│ %-25s │ %8.3f │ %7.3f │   %.2f×  │\n",
               method_names[i], times[i] * 1000, gflops[i], speedups[i]);
    }
    printf("└───────────────────────────┴──────────┴─────────┴──────────┘\n\n");
    
    // Find best
    int best_idx = 0;
    for (int i = 1; i < 5; i++) {
        if (gflops[i] > gflops[best_idx]) best_idx = i;
    }
    
    printf("BEST METHOD: %s\n", method_names[best_idx]);
    printf("  File: ");
    if (best_idx == 0) printf("csr_serial.c\n");
    else if (best_idx == 1) printf("csr_parallel.c\n");
    else if (best_idx == 2) printf("bcsr_parallel.c\n");
    else if (best_idx == 3) printf("bucket_parallel.c\n");
    else printf("bcsr_bucket_parallel.c\n");
    printf("  Performance: %.3f GFlop/s\n", gflops[best_idx]);
    printf("  Speedup: %.2f×\n\n", speedups[best_idx]);
    
    double roofline = 52.0 * 0.125;
    printf("ROOFLINE ANALYSIS:\n");
    printf("  • Peak: %.2f GFlop/s\n", roofline);
    printf("  • Best efficiency: %.1f%%\n\n", 100.0 * gflops[best_idx] / roofline);
    
    printf("========================================\n");
    printf("Next: Generate plots\n");
    printf("Run: python3 plot_results.py\n");
    printf("========================================\n");
    
    // Cleanup
    csr_free(A_csr);
    bcsr_free(A_bcsr);
    free(x); free(y1); free(y2); free(y3); free(y4); free(y5);
    
    return 0;
}
