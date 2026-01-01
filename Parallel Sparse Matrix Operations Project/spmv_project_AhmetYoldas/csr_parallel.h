/**
 * METHOD 2: CSR Parallel
 * OpenMP parallelization with dynamic scheduling
 */

#ifndef CSR_PARALLEL_H
#define CSR_PARALLEL_H

#include "common.h"

/**
 * CSR Parallel SpMV
 * 
 * Optimizations:
 * - OpenMP parallel for
 * - Dynamic scheduling (chunk size 64)
 * - Load balancing for irregular rows
 * 
 * Expected performance: ~7-8 GFlop/s (8 threads)
 * Expected speedup: 4-5× vs serial
 */
void spmv_csr_parallel(const CSR_Matrix *A, const double *x, double *y);

#endif // CSR_PARALLEL_H
