#pragma once
#include "GMPtypes.h"

struct dMat *GMPmat2dMat(struct GMPmat *A);
struct GMPmat *dMat2GMPmat(struct dMat *A);
void mpz_to_mpq(mpq_t *rop, mpz_t *op, size_t m);
struct GMPmat *GMPmat_appendRow(struct GMPmat *A, mpq_t *row);
void mpz_norm(mpz_t norm, mpz_t *row, size_t m);
void mpq_row_init(mpq_t *row, size_t m);
void mpz_row_init(mpz_t *row, size_t m);
void mpq_row_clean(mpq_t *row, size_t m);
void mpz_row_clean(mpz_t *row, size_t m);