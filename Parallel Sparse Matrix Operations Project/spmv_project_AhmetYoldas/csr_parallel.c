/**
 * METHOD 2: CSR Parallel Implementation
 */

#include "csr_parallel.h"
#include <omp.h>

void spmv_csr_parallel(const CSR_Matrix *A, const double *x, double *y) {
    #pragma omp parallel for schedule(dynamic, 64)
    for (int i = 0; i < A->rows; i++) {
        double sum = 0.0;
        for (int k = A->row_ptr[i]; k < A->row_ptr[i+1]; k++) {
            sum += A->values[k] * x[A->col_idx[k]];
        }
        y[i] = sum;
    }
}
