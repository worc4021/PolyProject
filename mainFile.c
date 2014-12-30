#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <gmp.h>

#include "./../Mex/lrslib/lrslib.h"

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

int main(int argc, char const *argv[])
{
	int sizeInMB = 128;
	char *selector = "Init";

	
	if(strstr(argv[0], selector) != NULL) { /* This initialises the RAM disk */
		char holder[100];
		char helper[100];
		int factor = 2048;
		sprintf(helper,"%d", sizeInMB*factor);
		strcpy (holder,"diskutil erasevolume HFS+ 'MyDrive' `hdiutil attach -nomount ram://");
		strcat(holder, helper);
		strcat(holder, "`");
		if( access(path, F_OK) == -1 ) {
    		assert ( system( holder ) != -1 );
		} else {
    	assert( unmount(path, MNT_FORCE) != -1 );
    	}

	} else { /* This is the projection */

		int dim;
		struct dMat *fileout, *tmpMat;

		fileout = dMatFromFile(&dim);

		struct GMPmat *proj, *SET;

		SET = dMat2GMPmat(fileout);

		proj = projection(SET,dim);

		tmpMat = GMPmat2dMat(proj);


		toFile(tmpMat);


		dMat_destroy(fileout);
		dMat_destroy(tmpMat);
		GMPmat_destroy(proj);

	}
	return 0;
}