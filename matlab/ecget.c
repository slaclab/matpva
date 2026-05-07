
/* matlab wrapper for ecget */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include <mglue.h>
#include <ecget.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
int      namelen,i;
char     name[100];
long    *buf=0;
int      nelems=0;
mxArray  *mxa = 0;
double   *dptr;
LcaError theErr;

	mpvaMexGblInit();

	mpvaErrorInit(&theErr);

	LHSCHECK(nlhs, plhs);
    
	/* check for one argument of type string */
	if ( nrhs != 1 ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "usage:  data = ecget( PVName );\n" );
		goto cleanup;
	}
	if ( mxIsChar( prhs[0] ) != 1 ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "usage:  data = ecget( PVName );\n" );
		goto cleanup;
	}

    namelen = (mxGetM(prhs[0]) * mxGetN(prhs[0]) * sizeof(mxChar)) + 1;
    
    if (namelen>=sizeof(name)) {
        mpvaSetError(&theErr, EZCA_INVALIDARG, "PV name too long\n");
        goto cleanup;
    }
    
    mxGetString(prhs[0], name, namelen);
    
    ecdrget(name,&namelen,&buf,&nelems);

    if (!buf) {
        goto cleanup;
    }
    
    if (!(mxa = mxCreateDoubleMatrix(1,nelems,mxREAL))){
        mpvaFree(buf); buf=0;
        mpvaSetError(&theErr, EZCA_FAILEDMALLOC, "ecdrget: no memory");
        goto cleanup;
    }
    
    for (i=0, dptr=mxGetPr(mxa); i<nelems; i++, dptr++)
        *dptr = (double)buf[i];
    
    plhs[0] = mxa; mxa=0;
   
	nlhs = 0;

cleanup:
    if (buf) mpvaFree(buf);
	if (mxa) {
		mxDestroyArray(mxa);
	}
	ERR_CHECK(nlhs, plhs, &theErr);
}
