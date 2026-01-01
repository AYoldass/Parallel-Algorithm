/**
 * METHOD 3: BCSR Parallel
 * 4×4 blocking with OpenMP parallelization
 */

#ifndef BCSR_PARALLEL_H
#define BCSR_PARALLEL_H

#include "common.h"

/**
 * BCSR Parallel SpMV (4×4 blocks)
 * 
 * Optimizations:
 * - 4×4 register blocking
 * - Loop unrolling
 * - OpenMP parallelization
 * - SIMD-friendly memory access
 * 
 * Note: Performance depends on sparsity pattern
 * - Good for: Structured sparse (FEM, banded)
 * - Poor for: Random sparse (storage overhead)
 * 
 * Expected performance: Matrix-dependent
 */
void spmv_bcsr_parallel(const BCSR_Matrix *A, const double *x, double *y);

#endif // BCSR_PARALLEL_H
