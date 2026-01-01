/**
 * Common Definitions and Utilities
 * Shared across all SpMV methods
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ============================================
// CSR Matrix Format
// ============================================
typedef struct {
    int rows;
    int cols;
    int nnz;
    int *row_ptr;      // Size: rows+1
    int *col_idx;      // Size: nnz
    double *values;    // Size: nnz
} CSR_Matrix;

// ============================================
// BCSR Matrix Format (4×4 blocks)
// ============================================
typedef struct {
    int rows;
    int cols;
    int block_rows;
    int block_cols;
    int num_blocks;
    int *block_row_ptr;   // Size: block_rows+1
    int *block_col_idx;   // Size: num_blocks
    double *block_val;    // Size: num_blocks × 16
} BCSR_Matrix;

// ============================================
// Matrix Memory Management
// ============================================

// Allocate CSR matrix
CSR_Matrix* csr_alloc(int rows, int cols, int nnz);

// Free CSR matrix
void csr_free(CSR_Matrix *A);

// Generate random CSR matrix
CSR_Matrix* csr_random(int n, double density);

// Convert CSR to BCSR (4×4)
BCSR_Matrix* csr_to_bcsr(const CSR_Matrix *A);

// Free BCSR matrix
void bcsr_free(BCSR_Matrix *A);

#endif // COMMON_H
