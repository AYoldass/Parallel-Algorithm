/**
 * METHOD 4: CSR+Bucket Parallel (OPTIMIZED)
 * Adaptive bucket size for optimal parallelism
 */

#ifndef BUCKET_PARALLEL_H
#define BUCKET_PARALLEL_H

#include "common.h"

/**
 * CSR+Bucket Parallel SpMV (OPTIMIZED)
 * 
 * Optimizations:
 * - ADAPTIVE bucket size (based on threads and matrix size)
 * - Ensures 4× more buckets than threads (good load balance)
 * - Min bucket: 32 rows (cache locality)
 * - Max bucket: 512 rows (fits in L2)
 * - OpenMP parallelization
 * - Dynamic scheduling across buckets
 * 
 * Adaptive strategy:
 * - Small matrices (2K): 32-64 rows/bucket → 32+ buckets
 * - Large matrices (100K): 512 rows/bucket → 200+ buckets
 * - Always ensures threads * 4 buckets minimum
 * 
 * Expected performance: Matrix size-dependent
 * - 2K×2K: ~3-4 GFlop/s (improved from 1.6!)
 * - 100K×100K: Better cache utilization
 */
void spmv_bucket_parallel(const CSR_Matrix *A, const double *x, double *y);

#endif // BUCKET_PARALLEL_H
