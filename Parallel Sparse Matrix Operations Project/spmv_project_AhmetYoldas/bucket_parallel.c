/**
 * METHOD 4: CSR+Bucket Parallel Implementation (OPTIMIZED)
 * Adaptive bucket size for better parallelism
 */

#include "bucket_parallel.h"
#include <omp.h>

void spmv_bucket_parallel(const CSR_Matrix *A, const double *x, double *y) {
    // ADAPTIVE BUCKET SIZE for better parallelism
    // Goal: Create enough buckets for all threads
    
    int num_threads = omp_get_max_threads();
    
    // Strategy: Ensure at least 4× more buckets than threads
    // This allows good load balancing with dynamic scheduling
    int min_buckets = num_threads * 4;
    
    // Calculate bucket size
    // Min bucket size: 32 rows (good cache locality)
    // Max bucket size: 512 rows (still fits in L2)
    int bucket_size = A->rows / min_buckets;
    if (bucket_size < 32) bucket_size = 32;
    if (bucket_size > 512) bucket_size = 512;
    
    int num_buckets = (A->rows + bucket_size - 1) / bucket_size;
    
    memset(y, 0, A->rows * sizeof(double));
    
    // Process each bucket in parallel
    // Dynamic scheduling: chunk size = 1 (each bucket is a task)
    #pragma omp parallel for schedule(dynamic, 1)
    for (int bucket_id = 0; bucket_id < num_buckets; bucket_id++) {
        int bucket_start = bucket_id * bucket_size;
        int bucket_end = (bucket_start + bucket_size < A->rows) ? 
                         bucket_start + bucket_size : A->rows;
        
        // Process all rows in this bucket
        // Cache-friendly: All y[bucket_start:bucket_end] stays in L2
        for (int i = bucket_start; i < bucket_end; i++) {
            double sum = 0.0;
            
            // Inner loop: x vector elements accessed
            for (int k = A->row_ptr[i]; k < A->row_ptr[i+1]; k++) {
                sum += A->values[k] * x[A->col_idx[k]];
            }
            
            y[i] = sum;
        }
    }
}
