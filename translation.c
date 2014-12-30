#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <gmp.h>
#include "GMPmat.h"
#include "dMat.h"
#include "./../Mex/lrslib/lrslib.h"
#include "translation.h"

void mpz_row_clean(mpz_t *row, size_t m)
{
    assert(row != NULL);
    for (size_t i = 0; i < m; ++i)
    {
        mpz_clear(row[i]);
    }
    free(row);
}

void mpq_row_clean(mpq_t *row, size_t m)
{
    assert(row != NULL);
    for (size_t i = 0; i < m; ++i)
    {
        mpq_clear(row[i]);
    }
    free(row);
}

void mpz_row_init(mpz_t *row, size_t m)
{
    assert(row != NULL);
    for (size_t i = 0; i < m; ++i)
    {
        mpz_init(row[i]);
    }
}

void mpq_row_init(mpq_t *row, size_t m)
{
    assert(row != NULL);
    for (size_t i = 0; i < m; ++i)
    {
        mpq_init(row[i]);
    }
}

void mpz_norm(mpz_t norm, mpz_t *row, size_t m)
{
    assert( row != NULL );
    mpz_t help;
    mpz_init(help);
    for (size_t i = 0; i < m; ++i)
    {
        mpz_addmul(help, row[i], row[i]);
    }
    mpz_sqrt(norm, help);
    mpz_clear(help);
}

struct GMPmat *GMPmat_appendRow(struct GMPmat *A, mpq_t *row)
{
    assert( A != NULL && row != NULL );
    struct GMPmat *retVal;
    size_t m,n, i, j;
    m = GMPmat_Rows(A) + 1;
    n = GMPmat_Cols(A);
    retVal = GMPmat_create( m, n, 0);
    assert( m != 0 );

    for (i = 0; i < (m-1); ++i)
    {
        for (j = 0; j < n; ++j)
        {
            mpq_init(retVal->data[i*n + j]);
            mpq_set(retVal->data[i*n + j],A->data[i*n + j]);
        }
    }
    for (i = 0; i < n; ++i)
    {
        mpq_init(retVal->data[(m-1)*n + i]);
        mpq_set(retVal->data[(m-1)*n + i], row[i]);
    }
    GMPmat_destroy(A);
    return retVal;
}

void mpz_to_mpq(mpq_t *rop, mpz_t *op, size_t m)
{
    size_t i;
    assert( rop != NULL && op != NULL);
    if (zero(op[0]))
    {
        mpz_t norm;
        mpz_init(norm);
        mpz_norm(norm, op, m);
        for (i = 0; i < m; ++i)
        {
            mpq_set_num(rop[i], op[i]);
            mpq_set_den(rop[i], norm);
            mpq_canonicalize(rop[i]);
        }
        mpz_clear(norm);
    }else{
        for (i = 0; i < m; ++i)
        {
            mpq_set_num(rop[i], op[i]);
            mpq_set_den(rop[i], op[0]);
            mpq_canonicalize(rop[i]);
        }
    }
}

struct GMPmat *dMat2GMPmat(struct dMat *A)
{
    struct GMPmat *retVal;
    retVal = GMPmat_create(dMat_Rows(A),dMat_Cols(A),0);
    for (size_t i = 0; i < dMat_Rows(A); ++i)
    {
        for (size_t j = 0; j < dMat_Cols(A); ++j)
        {
            mpq_init(retVal->data[i*dMat_Cols(A)+j]);
            mpq_set_d(retVal->data[i*dMat_Cols(A)+j], dMat_getValue(A, i, j));
        }
    }
    return retVal;
}

struct dMat *GMPmat2dMat(struct GMPmat *A)
{
    struct dMat *retVal;
    retVal = dMat_create(GMPmat_Rows(A),GMPmat_Cols(A),0);
    for (size_t i = 0; i < GMPmat_Rows(A); ++i)
    {
        for (size_t j = 0; j < GMPmat_Cols(A); ++j)
        {
            dMat_setValue(retVal, i, j, mpq_get_d(A->data[i*GMPmat_Cols(A)+j]));
        }
    }
    return retVal;
}