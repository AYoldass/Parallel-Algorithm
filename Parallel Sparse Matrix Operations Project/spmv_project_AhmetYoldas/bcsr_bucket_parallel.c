/**
 * METHOD 5: BCSR+Bucket Parallel Implementation (HYBRID - OPTIMIZED)
 * Adaptive bucket size for better parallelism
 */

#include "bcsr_bucket_parallel.h"
#include <omp.h>

void spmv_bcsr_bucket_parallel(const BCSR_Matrix *A, const double *x, double *y) {
    // ADAPTIVE BUCKET SIZE for block rows
    int num_threads = omp_get_max_threads();
    
    // Ensure at least 4× more buckets than threads
    int min_buckets = num_threads * 4;
    
    // Calculate bucket size in block rows
    // Min: 8 block rows (32 actual rows)
    // Max: 128 block rows (512 actual rows)
    int bucket_size = A->block_rows / min_buckets;
    if (bucket_size < 8) bucket_size = 8;
    if (bucket_size > 128) bucket_size = 128;
    
    int num_buckets = (A->block_rows + bucket_size - 1) / bucket_size;
    
    memset(y, 0, A->rows * sizeof(double));
    
    // Process each bucket of block rows in parallel
    #pragma omp parallel for schedule(dynamic, 1)
    for (int bucket_id = 0; bucket_id < num_buckets; bucket_id++) {
        int bucket_start = bucket_id * bucket_size;
        int bucket_end = (bucket_start + bucket_size < A->block_rows) ? 
                         bucket_start + bucket_size : A->block_rows;
        
        // Process all block rows in this bucket
        // All y[bucket_start*4 : bucket_end*4] stays in L2 cache
        for (int br = bucket_start; br < bucket_end; br++) {
            int row_start = br * 4;
            
            // BCSR 4×4 blocked computation
            for (int kb = A->block_row_ptr[br]; kb < A->block_row_ptr[br + 1]; kb++) {
                int bc = A->block_col_idx[kb];
                int col_start = bc * 4;
                const double *block = &A->block_val[kb * 16];
                
                // Register blocking: load 4 x values once
                double x0 = (col_start + 0 < A->cols) ? x[col_start + 0] : 0.0;
                double x1 = (col_start + 1 < A->cols) ? x[col_start + 1] : 0.0;
                double x2 = (col_start + 2 < A->cols) ? x[col_start + 2] : 0.0;
                double x3 = (col_start + 3 < A->cols) ? x[col_start + 3] : 0.0;
                
                // Fully unrolled 4×4 multiplication
                if (row_start + 0 < A->rows) {
                    y[row_start + 0] += block[0] * x0 + block[1] * x1 + 
                                         block[2] * x2 + block[3] * x3;
                }
                if (row_start + 1 < A->rows) {
                    y[row_start + 1] += block[4] * x0 + block[5] * x1 + 
                                         block[6] * x2 + block[7] * x3;
                }
                if (row_start + 2 < A->rows) {
                    y[row_start + 2] += block[8] * x0 + block[9] * x1 + 
                                         block[10] * x2 + block[11] * x3;
                }
                if (row_start + 3 < A->rows) {
                    y[row_start + 3] += block[12] * x0 + block[13] * x1 + 
                                         block[14] * x2 + block[15] * x3;
                }
            }
        }
    }
}
