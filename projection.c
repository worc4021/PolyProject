#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <gmp.h>
#include "./../Mex/lrslib/lrslib.h"
#include "GMPmat.h"
#include "translation.h"
#include "projection.h"



struct GMPmat *projection(struct GMPmat *inp, int d)
{

    lrs_dic *Pv, *Ph; /* structure for holding current dictionary and indices  */
    lrs_dat *Qv, *Qh; /* structure for holding static problem data               */
    lrs_mp_vector output; /* one line of output:ray,vertex,facet,linearity */
    lrs_mp_matrix Lin;    /* holds input linearities if any are found      */

      size_t i, j, cols, rows;
      long col;     /* output column index for dictionary            */

      /* Global initialization - done once */

      assert( lrs_init ("\n lrsTrial:"));

      Qv = lrs_alloc_dat ("LRS globals");
      assert( Qv!= NULL );

      Qv->m = GMPmat_Rows(inp);
      Qv->n = GMPmat_Cols(inp);

      output = lrs_alloc_mp_vector (Qv->n);

      lrs_mp_vector num, den;
      num = lrs_alloc_mp_vector(GMPmat_Cols(inp));
      den = lrs_alloc_mp_vector(GMPmat_Cols(inp));

      Pv = lrs_alloc_dic (Qv);   /* allocate and initialize lrs_dic      */
      assert( Pv != NULL );

      
      struct GMPmat *Helper;
      Helper = GMPmat_create(0, GMPmat_Cols(inp), 1);
      
      mpq_t *curRow;
      curRow = calloc(GMPmat_Cols(inp), sizeof(mpq_t));
      assert( curRow != NULL );
      mpq_row_init(curRow, GMPmat_Cols(inp));



      for (i = 1; i <= GMPmat_Rows(inp); ++i)
      {
        GMPmat_getRow(num, den, inp, i-1);
        lrs_set_row_mp(Pv,Qv,i,num,den,GE);
      }

      assert( lrs_getfirstbasis (&Pv, Qv, &Lin, FALSE) );
      printf("\n");


      for (col = 0L; col < Qv->nredundcol; col++)  /* print linearity space */
        lrs_printoutput (Qv, Lin[col]); 

      do
        {
          for (col = 0L; col <= Pv->d; col++)
            if (lrs_getsolution (Pv, Qv, output, col)) {
              mpz_to_mpq(curRow, output, GMPmat_Cols(Helper));
              Helper = GMPmat_appendRow(Helper, curRow);
              // lrs_printoutput(Qv, output);
            }
        }
        while (lrs_getnextbasis (&Pv, Qv, FALSE));

        mpq_row_clean(curRow, GMPmat_Cols(Helper));
        lrs_clear_mp_vector (output, Qv->n);
        lrs_clear_mp_vector (num, Qv->n);
        lrs_clear_mp_vector (den, Qv->n);
        lrs_free_dic (Pv,Qv);       /* deallocate lrs_dic */
        lrs_free_dat (Qv);          /* deallocate lrs_dat */
     
        Qh = lrs_alloc_dat ("LRS globals");
        assert( Qh != NULL );

        Qh->m = GMPmat_Rows(Helper);
        Qh->n = GMPmat_Cols(Helper) - d;

        Qh->hull = TRUE;     /* convex hull problem: facet enumeration      */
        Qh->polytope = TRUE;  /* input is a polytope                         */
        // Qh->debug = TRUE;

        output = lrs_alloc_mp_vector (Qh->n);
        num = lrs_alloc_mp_vector (Qh->n);
        den = lrs_alloc_mp_vector (Qh->n);

        Ph = lrs_alloc_dic (Qh);
        assert( Ph != NULL );
        
        struct GMPmat *retVal;
        retVal = GMPmat_create(0, Qh->n, 0);
        
        rows = GMPmat_Rows (Helper);
        cols = GMPmat_Cols (retVal);

        curRow = calloc(cols, sizeof(mpq_t));
        assert( curRow != NULL );
        mpq_row_init(curRow, cols);

        mpq_t curVal;
        mpq_init(curVal);

       for (i = 0; i < rows; ++i)
       {
         for (j = 0; j < cols; ++j)
          {
            GMPmat_getValue (curVal, Helper, i, j);
            mpz_set (num[j], mpq_numref(curVal));
            mpz_set (den[j], mpq_denref(curVal));
          }
          lrs_set_row_mp (Ph, Qh, i+1 ,num, den, GE);
        }

        mpq_clear(curVal);

        assert( lrs_getfirstbasis (&Ph, Qh, &Lin, FALSE) );

        for (col = 0L; col < Qh->nredundcol; col++)  /* print linearity space */
          lrs_printoutput (Qh, Lin[col]);

        do
        {
          for (col = 0L; col <= Ph->d; col++)
            if (lrs_getsolution (Ph, Qh, output, col)){
              mpz_to_mpq(curRow, output, GMPmat_Cols(retVal));
              retVal = GMPmat_appendRow(retVal, curRow);
              // lrs_printoutput(Qh, output);
            }
        }
        while (lrs_getnextbasis (&Ph, Qh, FALSE));

        GMPmat_destroy(Helper);
        mpq_row_clean(curRow, GMPmat_Cols(retVal));
        lrs_clear_mp_vector (output, Qh->n);
        lrs_clear_mp_vector (num, Qh->n);
        lrs_clear_mp_vector (den, Qh->n);
        lrs_free_dic (Ph,Qh);
        lrs_free_dat (Qh);

        lrs_close ("lrsTrial:");
        printf ("\n");       

  GMPmat_destroy(inp);
  return retVal;
}

struct GMPmat *H2V(struct GMPmat *inp)
{

    lrs_dic *Pv, *Ph; /* structure for holding current dictionary and indices  */
    lrs_dat *Qv, *Qh; /* structure for holding static problem data               */
    lrs_mp_vector output; /* one line of output:ray,vertex,facet,linearity */
    lrs_mp_matrix Lin;    /* holds input linearities if any are found      */

      size_t i, j, cols, rows;
      long col;     /* output column index for dictionary            */

      /* Global initialization - done once */

      assert( lrs_init ("\n lrsTrial:"));

      Qv = lrs_alloc_dat ("LRS globals");
      assert( Qv!= NULL );

      Qv->m = GMPmat_Rows(inp);
      Qv->n = GMPmat_Cols(inp);

      output = lrs_alloc_mp_vector (Qv->n);

      lrs_mp_vector num, den;
      num = lrs_alloc_mp_vector(GMPmat_Cols(inp));
      den = lrs_alloc_mp_vector(GMPmat_Cols(inp));

      Pv = lrs_alloc_dic (Qv);   /* allocate and initialize lrs_dic      */
      assert( Pv != NULL );

      
      struct GMPmat *Helper;
      Helper = GMPmat_create(0, GMPmat_Cols(inp), 1);
      
      mpq_t *curRow;
      curRow = calloc(GMPmat_Cols(inp), sizeof(mpq_t));
      assert( curRow != NULL );
      mpq_row_init(curRow, GMPmat_Cols(inp));



      for (i = 1; i <= GMPmat_Rows(inp); ++i)
      {
        GMPmat_getRow(num, den, inp, i-1);
        lrs_set_row_mp(Pv,Qv,i,num,den,GE);
      }

      assert( lrs_getfirstbasis (&Pv, Qv, &Lin, FALSE) );
      printf("\n");


      for (col = 0L; col < Qv->nredundcol; col++)  /* print linearity space */
        lrs_printoutput (Qv, Lin[col]); 

      do
        {
          for (col = 0L; col <= Pv->d; col++)
            if (lrs_getsolution (Pv, Qv, output, col)) {
              mpz_to_mpq(curRow, output, GMPmat_Cols(Helper));
              Helper = GMPmat_appendRow(Helper, curRow);
            }
        }
        while (lrs_getnextbasis (&Pv, Qv, FALSE));

        mpq_row_clean(curRow, GMPmat_Cols(Helper));
        lrs_clear_mp_vector (output, Qv->n);
        lrs_clear_mp_vector (num, Qv->n);
        lrs_clear_mp_vector (den, Qv->n);
        lrs_free_dic (Pv,Qv);       /* deallocate lrs_dic */
        lrs_free_dat (Qv);          /* deallocate lrs_dat */

        GMPmat_destroy(inp);
        return Helper;
}

struct GMPmat *V2H(struct GMPmat *inp) /* This function is untested */
{
    lrs_dic *P;
    lrs_dat *Q;
    lrs_mp_vector output;
    lrs_mp_matrix Lin;

    long i;
    long col;

    assert( lrs_init ("lrsTrial:") );

    Q = lrs_alloc_dat ("LRS globals");
    assert ( Q != NULL );
    Q->m = GMPmat_Rows(inp);
    Q->n = GMPmat_Cols(inp);
    Q->hull = TRUE;
    Q->polytope = TRUE;

    output = lrs_alloc_mp_vector (Q->n);

    lrs_mp_vector num, den;
    num = lrs_alloc_mp_vector(GMPmat_Cols(inp));
    den = lrs_alloc_mp_vector(GMPmat_Cols(inp));

    P = lrs_alloc_dic (Q);
    assert ( P != NULL );

    struct GMPmat *retMat;
    retMat = GMPmat_create(0, GMPmat_Cols(inp), 1);
      
    mpq_t *curRow;
    curRow = calloc(GMPmat_Cols(inp), sizeof(mpq_t));
    assert( curRow != NULL );
    mpq_row_init(curRow, GMPmat_Cols(inp));


    for (i = 1; i <= GMPmat_Rows(inp); ++i)
    {
      GMPmat_getRow(num, den, inp, i-1);
      lrs_set_row_mp(P ,Q ,i ,num ,den , GE);
    }

    assert ( lrs_getfirstbasis (&P, Q, &Lin, FALSE) );
    lrs_printoutput (Q, Lin[col]);

    do
    {
      for (col = 0; col <= P->d; col++)
        if (lrs_getsolution (P, Q, output, col)){
          mpz_to_mpq(curRow, output, GMPmat_Cols(retMat));
          retMat = GMPmat_appendRow(retMat, curRow);
        }
    }
    while (lrs_getnextbasis (&P, Q, FALSE));

    mpq_row_clean( curRow, GMPmat_Cols(retMat) );
    lrs_clear_mp_vector ( output, Q->n);
    lrs_clear_mp_vector ( num, Q->n);
    lrs_clear_mp_vector ( den, Q->n);
    lrs_free_dic ( P , Q);
    lrs_free_dat ( Q );

    lrs_close ("lrsTrial:");

    GMPmat_destroy(inp);
    return retMat;

}