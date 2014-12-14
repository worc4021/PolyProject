#include "myFunctions.h"

#ifdef LRSLIB

struct rat *intToRat (lrs_mp Nin, lrs_mp Din)
{
  struct rat *retVal;
  mpq_t rop;
  char *strT;
  char strD[10000];
  char strN[10000];
  char strR[25000];
  
  strT = mpz_get_str (strN, 2, Nin);
  assert ( strT != NULL );

  strT = mpz_get_str (strD, 2, Din);
  assert ( strT != NULL );
  
  strcpy(strR, strN);
  strcat(strR, "/");
  strcat(strR, strD);
  
  mpq_init(rop);
  assert( mpq_set_str (rop, strR, 2) == 0);
  mpq_canonicalize( rop );

  retVal = d2r( mpq_get_d( rop ) );
  mpq_clear(rop);
  return retVal;
} 

void normCompute(lrs_mp rop, lrs_mp_vector vec, size_t m)
{
  lrs_mp norm;
  lrs_alloc_mp(norm);
  for (int i = 1; i < m; ++i)
  {
    mpz_addmul (norm, vec[i], vec[i]);
  }
  mpz_sqrt ( rop, norm );
  lrs_clear_mp(norm);
}

struct Matrix *matrix_appendRow(struct Matrix *A, lrs_mp_vector output){
  assert( A != NULL );
  struct Matrix *retVal;
  struct rat *tempRat;
  size_t n = matrix_rows(A);
  size_t m = matrix_cols(A);
  retVal = matrix_create(n + 1, m, 0);
  memcpy(retVal->num, A->num, m*n*sizeof(K));
  memcpy(retVal->den, A->den, m*n*sizeof(K));
  if( zero(output[0]) ){
    lrs_mp temp;
    lrs_alloc_mp( temp );
    normCompute(temp, output, m);
    for (int i = 0; i < m; ++i)
    {
      tempRat = intToRat( output[i], temp );
      matrix_setNumValue(retVal, n, i, tempRat->num);
      matrix_setDenValue(retVal, n, i, tempRat->den);
    }
  }else{
  for (int i = 0; i < m; ++i)
  {
    tempRat = intToRat( output[i], output[0] );
    matrix_setNumValue(retVal, n, i, tempRat->num);
    matrix_setDenValue(retVal, n, i, tempRat->den);
  }}
  return retVal;
}

struct Matrix *project(struct dMat *inp, int d){

    lrs_dic *Pv, *Ph; /* structure for holding current dictionary and indices  */
    lrs_dat *Qv, *Qh; /* structure for holding static problem data             */
    lrs_mp_vector output; /* one line of output:ray,vertex,facet,linearity */
    lrs_mp_matrix Lin;    /* holds input linearities if any are found      */

      long i;
      long col;     /* output column index for dictionary            */

      /* Global initialization - done once */

      assert( lrs_init ("\n lrsTrial:"));


      Qv = lrs_alloc_dat ("LRS globals");
      assert( Qv!= NULL );

      Qv->m = dMat_Rows(inp);
      Qv->n = dMat_Cols(inp);

      output = lrs_alloc_mp_vector (Qv->n);

      Pv = lrs_alloc_dic (Qv);   /* allocate and initialize lrs_dic      */
      assert( Pv != NULL );

      struct Matrix *ratMat;
      ratMat = dMat2Matrix(inp);
      struct rat *temp;
      long num[MAXCOL];
      long den[MAXCOL];

      struct Matrix *tester;
      tester = matrix_create(0, dMat_Cols(inp), 0);

      for (int i = 1; i <= dMat_Rows(inp); ++i)
      {
        memcpy(num, ratMat->num + (i-1)*matrix_cols(ratMat), matrix_cols(ratMat)*sizeof(*ratMat->num));
        memcpy(den, ratMat->den + (i-1)*matrix_cols(ratMat), matrix_cols(ratMat)*sizeof(*ratMat->den));
        lrs_set_row(Pv,Qv,i,num,den,GE);
      }

      assert( lrs_getfirstbasis (&Pv, Qv, &Lin, FALSE) );
      printf("\n");


      for (long col = 0L; col < Qv->nredundcol; col++)  /* print linearity space */
          lrs_printoutput (Qv, Lin[col]); 

      do
        {
          for (long col = 0; col <= Pv->d; col++)
            if (lrs_getsolution (Pv, Qv, output, col)) {
              #ifdef DBG
              lrs_printoutput (Qv, output);
              #endif /* DBG */
              tester = matrix_appendRow(tester, output);
            }
        }
        while (lrs_getnextbasis (&Pv, Qv, FALSE));

      /* free space : do not change order of next 3 lines! */

       lrs_clear_mp_vector (output, Qv->n);
       lrs_free_dic (Pv,Qv);           /* deallocate lrs_dic */
       lrs_free_dat (Qv);             /* deallocate lrs_dat */
     
        Qh = lrs_alloc_dat ("LRS globals");
        assert( Qh != NULL );

        Qh->n = matrix_cols(tester) - d;
        Qh->m = matrix_rows(tester);

        Qh->hull = TRUE;     /* convex hull problem: facet enumeration      */
        Qh->polytope= TRUE;  /* input is a polytope                         */
        Qh->getvolume= FALSE; /* compute the volume                          */

        output = lrs_alloc_mp_vector (Qh->n);

        Ph = lrs_alloc_dic (Qh);
        assert( Ph != NULL );

        #ifdef DBG
        printf("\n");
        dMat_print(Matrix2dMat(tester));
        #endif /* DBG */

        long numerator[MAXCOL];
        long denominator[MAXCOL];
        struct Matrix *retVal;
        retVal = matrix_create(0, Qh->n, 0);

       for (int i = 0; i != matrix_rows(tester); ++i)
       {
         memcpy(numerator, tester->num + i*tester->n, (Qh->m)*sizeof(K));
         memcpy(denominator, tester->den + i*tester->n, (Qh->m)*sizeof(K));
         lrs_set_row(Ph,Qh, i+1 ,numerator, denominator, GE);
       }

       assert( lrs_getfirstbasis (&Ph, Qh, &Lin, FALSE) );

       for (col = 0L; col < Qh->nredundcol; col++)  /* print linearity space */
        lrs_printoutput (Qh, Lin[col]);

        do
        {
          for (col = 0; col <= Ph->d; col++)
            if (lrs_getsolution (Ph, Qh, output, col))
              #ifdef DBG
              lrs_printoutput (Qh, output);
              #endif /* DBG */
              retVal = matrix_appendRow(retVal, output);
        }
        while (lrs_getnextbasis (&Ph, Qh, FALSE));

        

        lrs_clear_mp_vector (output, Qh->n);
        lrs_free_dic (Ph,Qh);           /* deallocate lrs_dic */
        lrs_free_dat (Qh);

     lrs_close ("lrsTrial:");
     printf("\n");      

     return retVal;
}

void dMat_print(struct dMat *A){
  assert (A != NULL);  
  size_t m, n;
      m = dMat_Rows(A);
      n = dMat_Cols(A);
      printf("\n");
      for (int i = 0; i < m; ++i)        
      {
        for (int j = 0; j < n; ++j)
        {
          printf("%9.2e, ", dMat_getValue(A, i, j));
        }
        printf("\n");
      }
      printf("\n");
}

void matrix_printD(struct Matrix *A){ /* Print the values in double*/
  assert (A != NULL);  
  size_t m, n;
      m = matrix_rows(A);
      n = matrix_cols(A);
      double num, den;
      printf("\n");
      for (int i = 0; i < m; ++i)        
      {
        for (int j = 0; j < n; ++j)
        {
          num = (double)matrix_getNum(A, i,j);
          den = (double)matrix_getDen(A, i,j);
          printf("%9.2e, ", num/den);
        }
        printf("\n");
      }
      printf("\n");
}

void matrix_printR(struct Matrix *A){ /* Print the values in double*/
  assert (A != NULL);  
  size_t m, n;
      m = matrix_rows(A);
      n = matrix_cols(A);
      K num, den;
      printf("\n");
      for (int i = 0; i < m; ++i)        
      {
        for (int j = 0; j < n; ++j)
        {
          num = matrix_getNum(A, i,j);
          den = matrix_getDen(A, i,j);
          printf("%ld/%ld, ", num, den);
        }
        printf("\n");
      }
      printf("\n");
}

#endif /* LRSLIB */


#ifdef MATLAB_MEX_FILE
struct dMat *readMXArray(const mxArray *pm){
	struct dMat *retVal;
	retVal = malloc (sizeof (*retVal));
	assert (retVal != NULL);

	double *ptr = mxGetPr(pm);

	retVal->m = mxGetM(pm);
	retVal->n = mxGetN(pm);
	retVal->data = malloc (retVal->m * retVal->n *sizeof(*retVal->data));
	assert (retVal->data != NULL);
	for (int i = 0; i != dMat_Rows(retVal); ++i)
	{
		for (int j = 0; j != dMat_Cols(retVal); ++j)
		{
			dMat_setValue(retVal, i, j, ptr[i + j*dMat_Rows(retVal)]);
		}
	}
	return retVal;
}


mxArray *writeMXArray(const struct dMat *A){
	mxArray *retVal;
	double *ptr;
	retVal = mxCreateNumericMatrix(dMat_Rows(A), dMat_Cols(A), OUTPUT_PRECISION, mxREAL);
	ptr = mxGetPr(retVal);
	for (int i = 0; i != dMat_Rows(A); ++i)
	{
		for (int j = 0; j != dMat_Cols(A); ++j)
		{
			ptr[i + j*dMat_Rows(A)] = dMat_getValue(A, i, j);
		}
	}
	return retVal;
}
#endif /* MATLAB_MEX_FILE */

struct Matrix *dMat2Matrix(struct dMat *inp)
{
    struct Matrix *retVal;
    struct rat *a;
    retVal = matrix_create(dMat_Rows(inp), dMat_Cols(inp), 0);
    for (int i = 0; i < dMat_Rows(inp); ++i)
    {
      for (int j = 0; j < dMat_Cols(inp); ++j)
      {
        a = d2r(dMat_getValue(inp, i, j));
        matrix_setNumValue(retVal, i, j, a->num);
        matrix_setDenValue(retVal, i, j, a->den);
      }
    }
    return retVal;
}

struct dMat *Matrix2dMat(struct Matrix *inp)
{
  struct dMat *retVal;
  retVal = dMat_create(matrix_rows(inp), matrix_cols(inp), 0);
  for (int i = 0; i < matrix_rows(inp); ++i)
  {
    for (int j = 0; j < matrix_cols(inp); ++j)
    {
      dMat_setValue(retVal, i, j, (double)matrix_getNum(inp, i, j)/(double)matrix_getDen(inp, i, j));
    }
  }
  return retVal;
}

static void __dMat_validate_indices (const struct dMat *A, size_t r, size_t c)
     {
       assert (A != NULL);
       assert (r < A->m);
       assert (c < A->n);
     }

double dMat_getValue (const struct dMat *A, size_t r, size_t c)
{
       __dMat_validate_indices (A, r, c);

       return (A->data[r*(A->n) + c]);
}

void dMat_setValue (const struct dMat *A, size_t r, size_t c, double val)
{
       __dMat_validate_indices (A, r, c);

       A->data[r*(A->n) + c] = val;
}

size_t dMat_Cols (const struct dMat *A)
     {
       assert (A != NULL);

       return (A->n);
     }


size_t dMat_Rows (const struct dMat *A)
     {
       assert (A != NULL);

       return (A->m);
     }


struct dMat *dMat_create (size_t m, size_t n, int init)
     {
       struct dMat *A;
       size_t         i, j;

       A = malloc (sizeof (*A));
       assert (A != NULL);

       A->m    = m;
       A->n    = n;
       A->data = malloc (m*n*sizeof(*A->data));
       assert (A->data != NULL);

       if (init != 0)
       {
         for (i=0; i!=m; ++i){
           for (j=0; j!=n; ++j){
            dMat_setValue(A, i, j, 0);
          }
        }
       }

       return (A);
     }

struct rat *d2r(double input){
	double x;
	if (input < 0){
		x = -input;
	}
	if (input >0 ) {
		x = input;
	}
	struct rat *res;
	if (x > (double)1/(double)MAXDEN)
	{
	long m[2][2];
    m[0][0] = m[1][1] = 1;
    m[0][1] = m[1][0] = 0;
	long ai;
	long maxDen = MAXDEN;
	while (m[1][0] *  ( ai = (long)x ) + m[1][1] <= maxDen) {
        long t;
        t = m[0][0] * ai + m[0][1];
        m[0][1] = m[0][0];
        m[0][0] = t;
        t = m[1][0] * ai + m[1][1];
        m[1][1] = m[1][0];
        m[1][0] = t;
        if (x == (double)ai) break;    // AF: division by zero
        x = 1/(x - (double) ai);
        if(x>(double)0x7FFFFFFF) break;  // AF: representation failure
    } 
    if (input<0){
    	res = rat_create(-m[0][0], m[1][0]);
	}
	if (input >0 ) {
		res = rat_create(m[0][0], m[1][0]);
	}
    assert(res != NULL);
	}
	if (x < (double)1/(double)MAXDEN)
	{
		res = rat_create(0, 1);
	}
    return res;
}

struct rat *rat_create(long num, long den){
	struct rat *retVal;
	retVal = malloc(sizeof(*retVal));
	assert( retVal != NULL );
	retVal->num = num;
	retVal->den = den;
	return(retVal);
}

static void __matrix_validate_indices (const struct Matrix *A, size_t r, size_t c)
     {
       assert (A != NULL);
       assert (r < A->m);
       assert (c < A->n);
     }




K matrix_getNum (const struct Matrix *A, size_t r, size_t c){
       __matrix_validate_indices (A, r, c);

       return (A->num[r*(A->n) + c]);
}

K matrix_getDen (const struct Matrix *A, size_t r, size_t c){
       __matrix_validate_indices (A, r, c);

       return (A->den[r*(A->n) + c]);
}



size_t matrix_cols (const struct Matrix *A)
     {
       assert (A != NULL);

       return (A->n);
     }

size_t matrix_rows (const struct Matrix *A)
     {
       assert (A != NULL);

       return (A->m);
     }


void matrix_setNumValue (struct Matrix *A, size_t r, size_t c, K x){
       __matrix_validate_indices (A, r, c);

       A->num[r*(A->n) + c] = x;
}

void matrix_setDenValue (struct Matrix *A, size_t r, size_t c, K x){
       __matrix_validate_indices (A, r, c);

       A->den[r*(A->n) + c] = x;
}


struct Matrix *matrix_create (size_t m, size_t n, int init)
     {
       struct Matrix *A;
       size_t         i, j;

       A = malloc (sizeof (*A));
       assert (A != NULL);

       A->m    = m;
       A->n    = n;
       A->num = malloc (m*n*sizeof(*A->num));
       A->den = malloc (m*n*sizeof(*A->den));
       assert (A->num != NULL && A->den != NULL);

       if (init != 0)
       {
         for (i=0; i!=m; ++i){
           for (j=0; j!=n; ++j){
            matrix_setNumValue(A, i, j, 0);
            matrix_setDenValue(A, i, j, 1);
          }
        }
       }

       return (A);
     }

void matrix_destroy (struct Matrix *A)
     {
       assert (A != NULL);
       if (A->num != NULL) free (A->num);
       if (A->den != NULL) free (A->den);
       A->num = NULL;
       A->den = NULL;
       A->n   = 0;
       A->m   = 0;
       free (A);
     }


struct dMat *dMatFromFile(int *dim){
  FILE *fp;
  fp = fopen("MATLABtoLOWLEVEL","r");
  assert( fp != NULL );
  uint8_T out, m, n;
  assert( fread(&out ,1 , sizeof(uint8_T),fp) == sizeof(uint8_T) );
  assert( fread(&m ,1 , sizeof(uint8_T),fp) == sizeof(uint8_T) );
  assert( fread(&n ,1 , sizeof(uint8_T),fp) == sizeof(uint8_T) );
  *dim = (int)out;
  struct dMat *retVal;
  retVal = dMat_create((size_t)m, (size_t)n, 0);
  for (int i = 0; i != dMat_Cols(retVal); ++i)
  {
    for (int j = 0; j != dMat_Rows(retVal); ++j)
    {
      assert( fread(retVal->data + i + j*n,1 , sizeof(double),fp) == sizeof(double) );
    }
  }
  fclose(fp);
  return retVal;
}

void toFile(struct dMat *A)
{
  FILE *fp;
  fp = fopen("LOWLEVELtoMATLAB","w");
  assert( fp != NULL );
  uint8_T m = (uint8_T)dMat_Rows(A);
  uint8_T n = (uint8_T)dMat_Cols(A);
  assert( fwrite(&m , 1, sizeof(uint8_T), fp) == sizeof(uint8_T) );
  assert( fwrite(&n , 1, sizeof(uint8_T), fp) == sizeof(uint8_T) );
  for (int i = 0; i != dMat_Cols(A); ++i)
  {
    for (int j = 0; j != dMat_Rows(A); ++j)
    {
      assert( fwrite(A->data + i + j*n,1 , sizeof(double), fp) == sizeof(double) );
    }
  }
  fclose(fp);
}