/**
 * METHOD 5: BCSR+Bucket Parallel (HYBRID - OPTIMIZED)
 * Combines 4×4 blocking with adaptive bucket partitioning
 */

#ifndef BCSR_BUCKET_PARALLEL_H
#define BCSR_BUCKET_PARALLEL_H

#include "common.h"

/**
 * BCSR+Bucket Parallel SpMV (HYBRID METHOD - OPTIMIZED)
 * 
 * Combines two optimizations:
 * 1. BCSR 4×4 blocking (register blocking + SIMD)
 * 2. ADAPTIVE bucket partitioning (improved parallelism)
 * 
 * Implementation:
 * - Partition block rows into adaptive buckets
 * - Bucket size adapts to matrix size and thread count
 * - Each bucket processed independently
 * - Within each bucket: 4×4 blocked computation
 * - OpenMP parallelization across buckets
 * 
 * Optimizations:
 * - Register blocking (4×4)
 * - Loop unrolling
 * - Adaptive bucket size (8-128 block rows)
 * - Ensures 4× more buckets than threads
 * - SIMD-friendly access patterns
 * - Parallel execution
 * 
 * Expected behavior (OPTIMIZED):
 * - Better parallelism than fixed bucket size
 * - Still affected by BCSR storage overhead (11×)
 * - Performance: Matrix-dependent
 * - Small random sparse: May still underperform
 * - Large structured sparse: Potential improvement
 * 
 * This is an EXPERIMENTAL hybrid method!
 */
void spmv_bcsr_bucket_parallel(const BCSR_Matrix *A, const double *x, double *y);

#endif // BCSR_BUCKET_PARALLEL_H
