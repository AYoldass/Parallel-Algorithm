/**
 * Common Utilities Implementation
 */

#include "common.h"

// ============================================
// CSR Memory Management
// ============================================

CSR_Matrix* csr_alloc(int rows, int cols, int nnz) {
    CSR_Matrix *A = (CSR_Matrix*)malloc(sizeof(CSR_Matrix));
    A->rows = rows;
    A->cols = cols;
    A->nnz = nnz;
    A->row_ptr = (int*)calloc(rows + 1, sizeof(int));
    A->col_idx = (int*)malloc(nnz * sizeof(int));
    A->values = (double*)malloc(nnz * sizeof(double));
    return A;
}

void csr_free(CSR_Matrix *A) {
    if (A) {
        free(A->row_ptr);
        free(A->col_idx);
        free(A->values);
        free(A);
    }
}

CSR_Matrix* csr_random(int n, double density) {
    int nnz_estimate = (int)(n * n * density * 1.2);
    CSR_Matrix *A = csr_alloc(n, n, nnz_estimate);
    
    int nnz_actual = 0;
    A->row_ptr[0] = 0;
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if ((double)rand() / RAND_MAX < density) {
                if (nnz_actual < nnz_estimate) {
                    A->col_idx[nnz_actual] = j;
                    A->values[nnz_actual] = (double)rand() / RAND_MAX;
                    nnz_actual++;
                }
            }
        }
        A->row_ptr[i + 1] = nnz_actual;
    }
    
    A->nnz = nnz_actual;
    return A;
}

// ============================================
// BCSR Conversion
// ============================================

BCSR_Matrix* csr_to_bcsr(const CSR_Matrix *A) {
    BCSR_Matrix *B = (BCSR_Matrix*)malloc(sizeof(BCSR_Matrix));
    
    B->rows = A->rows;
    B->cols = A->cols;
    B->block_rows = (A->rows + 3) / 4;
    B->block_cols = (A->cols + 3) / 4;
    
    // Count non-zero blocks
    int *block_nnz = (int*)calloc(B->block_rows, sizeof(int));
    
    for (int br = 0; br < B->block_rows; br++) {
        int *col_flags = (int*)calloc(B->block_cols, sizeof(int));
        
        for (int i = 0; i < 4 && (br * 4 + i) < A->rows; i++) {
            int row = br * 4 + i;
            for (int k = A->row_ptr[row]; k < A->row_ptr[row + 1]; k++) {
                int col = A->col_idx[k];
                int bc = col / 4;
                col_flags[bc] = 1;
            }
        }
        
        for (int bc = 0; bc < B->block_cols; bc++) {
            block_nnz[br] += col_flags[bc];
        }
        free(col_flags);
    }
    
    // Allocate BCSR
    B->num_blocks = 0;
    for (int br = 0; br < B->block_rows; br++) {
        B->num_blocks += block_nnz[br];
    }
    
    B->block_row_ptr = (int*)malloc((B->block_rows + 1) * sizeof(int));
    B->block_col_idx = (int*)malloc(B->num_blocks * sizeof(int));
    B->block_val = (double*)calloc(B->num_blocks * 16, sizeof(double));
    
    B->block_row_ptr[0] = 0;
    for (int br = 0; br < B->block_rows; br++) {
        B->block_row_ptr[br + 1] = B->block_row_ptr[br] + block_nnz[br];
    }
    
    // Fill blocks
    for (int br = 0; br < B->block_rows; br++) {
        int *col_map = (int*)malloc(B->block_cols * sizeof(int));
        for (int bc = 0; bc < B->block_cols; bc++) {
            col_map[bc] = -1;
        }
        
        int block_idx = B->block_row_ptr[br];
        
        for (int i = 0; i < 4 && (br * 4 + i) < A->rows; i++) {
            int row = br * 4 + i;
            for (int k = A->row_ptr[row]; k < A->row_ptr[row + 1]; k++) {
                int col = A->col_idx[k];
                int bc = col / 4;
                int j = col % 4;
                
                if (col_map[bc] == -1) {
                    col_map[bc] = block_idx;
                    B->block_col_idx[block_idx] = bc;
                    block_idx++;
                }
                
                int bidx = col_map[bc];
                B->block_val[bidx * 16 + i * 4 + j] = A->values[k];
            }
        }
        free(col_map);
    }
    
    free(block_nnz);
    return B;
}

void bcsr_free(BCSR_Matrix *A) {
    if (A) {
        free(A->block_row_ptr);
        free(A->block_col_idx);
        free(A->block_val);
        free(A);
    }
}
