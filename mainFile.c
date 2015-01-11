#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <gmp.h>
#include <time.h>

#include "lrslib/lrslib.h"

#include "GMPtypes.h"


/* Projection */
#include "projection.h"

/* dMat */
#include "dMat.h"

/* GMPmat */
#include "GMPmat.h"

/* translation */

#include "translation.h"

char *path = "/Volumes/MyDrive/";

mpz_t gONE, gZERO;

int main(int argc, char const *argv[])
{
	int sizeInMB = 128;

	mpz_init ( gZERO );
	mpz_init_set_str ( gONE, "1", 10 );

	int dim;
	struct dMat *fileout, *tmpMat;

	struct GMPmat *outset, *inset;

	clock_t entry, out;
	double cpu_time_used;


	if (argc > 1)
	{
		if ( !strcmp( argv[1], "init" ) ){
			char holder[100];
			char helper[100];
			int factor = 2048;
			sprintf(helper,"%d", sizeInMB*factor);
			strcpy (holder,"diskutil erasevolume HFS+ 'MyDrive' `hdiutil attach -nomount ram://");
			strcat(holder, helper);
			strcat(holder, "`");
			if( access(path, F_OK) == -1 ) {
	    		
	    		assert ( system( holder ) != -1 );
			
			} else 
	    		assert( unmount(path, MNT_FORCE) != -1 );
			
				return 0;
		}
		else if ( !strcmp( argv[1], "vertex" ))
		{
				entry = clock();
				fileout = dMatFromFile(&dim);
				inset = dMat2GMPmat(fileout);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Reading in data took %f seconds.\n", cpu_time_used);

				entry = clock();
				outset = H2V(inset);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Computing the vertices took %f seconds.\n", cpu_time_used);

				entry = clock();
				tmpMat = GMPmat2dMat( outset );
				toFile(tmpMat);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Writing output data took %f seconds.\n", cpu_time_used);

				dMat_destroy(fileout);
				dMat_destroy(tmpMat);
				GMPmat_destroy(outset);
				return 0;
		}
		else if ( !strcmp( argv[1], "reduce" )){
					
				entry = clock();
				fileout = dMatFromFile(&dim);
				inset = dMat2GMPmat(fileout);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Reading in data took %f seconds.\n", cpu_time_used);

				entry = clock();
				outset = reducemat(inset);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Computing minimal representation took %f seconds.\n", cpu_time_used);

				entry = clock();
				tmpMat = GMPmat2dMat( outset );
				toFile(tmpMat);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Writing output data took %f seconds.\n", cpu_time_used);

				dMat_destroy(fileout);
				dMat_destroy(tmpMat);
				GMPmat_destroy(outset);
				return 0;
		}
		else if ( !strcmp( argv[1], "project" )){

				entry = clock();
				fileout = dMatFromFile(&dim);
				inset = dMat2GMPmat(fileout);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Reading in data took %f seconds.\n", cpu_time_used);
				
				entry = clock();
				inset = H2V(inset);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Computing the vertices/rays took %f seconds.\n", cpu_time_used);

				entry = clock();
				inset = GMPmat_dropCols(inset, dim);
				inset = reducevertices(inset);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Computing minimal vertex/ray representation took %f seconds.\n", cpu_time_used);

				entry = clock();
				outset = V2H(inset);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Computing halfspace representation took %f seconds.\n", cpu_time_used);

				entry = clock();
				outset = reducemat(outset);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Computing minimal halfspace representation took %f seconds.\n", cpu_time_used);

				entry = clock();
				tmpMat = GMPmat2dMat( outset );
				toFile(tmpMat);
				out = clock();
				cpu_time_used = ((double) (out - entry)) / CLOCKS_PER_SEC;
				fprintf(stdout, "Writing output data took %f seconds.\n", cpu_time_used);

				dMat_destroy(fileout);
				dMat_destroy(tmpMat);
				GMPmat_destroy(outset);
				return 0;
		}
		else {
			fprintf(stdout, "Not enough arguments\n" );
			return 1;
		}
	} else 
		return 1;
}