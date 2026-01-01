/**
 * METHOD 3: BCSR Parallel Implementation
 */

#include "bcsr_parallel.h"
#include <omp.h>

void spmv_bcsr_parallel(const BCSR_Matrix *A, const double *x, double *y) {
    memset(y, 0, A->rows * sizeof(double));
    
    #pragma omp parallel for schedule(dynamic, 64)
    for (int br = 0; br < A->block_rows; br++) {
        int row_start = br * 4;
        
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
