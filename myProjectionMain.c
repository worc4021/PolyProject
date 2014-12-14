#include <assert.h>

#ifdef MATLAB_MEX_FILE
#include "mex.h"
#include "matrix.h"
#endif /* MATLAB_MEX_FILE */

#include "myFunctions.h"


#ifndef MATLAB_MEX_FILE

int main(int argc, char const *argv[])
{
    int dim;
    struct dMat *help;
    struct Matrix *outp;
    help = dMatFromFile(&dim);
  	outp = project(help, dim);
  	help = Matrix2dMat(outp);
    // dMat_print(help);
    toFile(help);

	return 0;
}


#endif /* MATLAB_MEX_FILE */

#ifdef MATLAB_MEX_FILE

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{

	if ( nrhs != 1 ) {
        mexErrMsgIdAndTxt("MATLAB:mexTest:rhs","One input only.");
    } 

    struct dMat *retMat;
    struct Matrix *helper;

    retMat = readMXArray(prhs[0]);
    // helper = project(retMat, 1);
    helper = dMat2Matrix(retMat);
    retMat = Matrix2dMat(helper);


    plhs[0] = writeMXArray(retMat);
}

#endif /* MATLAB_MEX_FILE */