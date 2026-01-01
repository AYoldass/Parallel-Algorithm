/**
 * METHOD 1: CSR Serial Implementation
 */

#include "csr_serial.h"

void spmv_csr_serial(const CSR_Matrix *A, const double *x, double *y) {
    for (int i = 0; i < A->rows; i++) {
        double sum = 0.0;
        for (int k = A->row_ptr[i]; k < A->row_ptr[i+1]; k++) {
            sum += A->values[k] * x[A->col_idx[k]];
        }
        y[i] = sum;
    }
}
