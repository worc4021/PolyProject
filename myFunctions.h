#ifdef MATLAB_MEX_FILE
#include "mex.h"
#include "matrix.h"
#endif /* MATLAB_MEX_FILE */

#include "tmwtypes.h" /* This is necessary to use uint8_T as the output of Matlab's uint8() command */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "./lrslib/lrslib.h"

#define OUTPUT_PRECISION mxDOUBLE_CLASS

#define MAXCOL 1000
#define MAXDEN 1000000000

typedef long K;

struct Matrix{
	K *num, *den;
	size_t m, n;
};

struct dMat{
	double *data;
	size_t m, n;
};

struct rat
{
	long num;
	long den;
};


#ifdef LRSLIB
struct rat *intToRat (lrs_mp Nin, lrs_mp Din);
void normCompute(lrs_mp rop, lrs_mp_vector vec, size_t m);
struct Matrix *matrix_appendRow(struct Matrix *A, lrs_mp_vector output);
struct Matrix *project(struct dMat *inp, int d);
void dMat_print(struct dMat *A);
void matrix_printD(struct Matrix *A);
void matrix_printR(struct Matrix *A);
#endif /* LRSLIB */

#ifdef MATLAB_MEX_FILE
struct dMat *readMXArray(const mxArray *pm);
mxArray *writeMXArray(const struct dMat *A);
#endif /* MATLAB_MEX_FILE */

struct Matrix *dMat2Matrix(struct dMat *inp);
struct dMat *Matrix2dMat(struct Matrix *inp);
static void __dMat_validate_indices (const struct dMat *A, size_t r, size_t c);
double dMat_getValue (const struct dMat *A, size_t r, size_t c);
void dMat_setValue (const struct dMat *A, size_t r, size_t c, double val);
size_t dMat_Cols (const struct dMat *A);
size_t dMat_Rows (const struct dMat *A);
struct dMat *dMat_create (size_t m, size_t n, int init);
struct rat *d2r(double input);
struct rat *rat_create(long num, long den);
static void __matrix_validate_indices (const struct Matrix *A, size_t r, size_t c);
K matrix_getNum (const struct Matrix *A, size_t r, size_t c);
K matrix_getDen (const struct Matrix *A, size_t r, size_t c);
size_t matrix_cols (const struct Matrix *A);
size_t matrix_rows (const struct Matrix *A);
void matrix_setNumValue (struct Matrix *A, size_t r, size_t c, K x);
void matrix_setDenValue (struct Matrix *A, size_t r, size_t c, K x);
struct Matrix *matrix_create (size_t m, size_t n, int init);
void matrix_destroy (struct Matrix *A);

struct dMat *dMatFromFile(int *dim);
void toFile(struct dMat *A);