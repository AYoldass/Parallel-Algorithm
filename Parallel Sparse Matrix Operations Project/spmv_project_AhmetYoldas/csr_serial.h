/**
 * METHOD 1: CSR Serial (Baseline)
 * Single-threaded, standard CSR format
 */

#ifndef CSR_SERIAL_H
#define CSR_SERIAL_H

#include "common.h"

/**
 * CSR Serial SpMV - Baseline
 * 
 * Features:
 * - Single-threaded execution
 * - Standard CSR format
 * - Simplest implementation
 * 
 * Expected performance: ~1-2 GFlop/s
 */
void spmv_csr_serial(const CSR_Matrix *A, const double *x, double *y);

#endif // CSR_SERIAL_H
