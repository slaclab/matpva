
/* matlab wrapper for ezcaGetRetryCount */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

#include <pvaClient.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
LcaError theErr;

	mpvaMexGblInit();

	mpvaErrorInit(&theErr);

	LHSCHECK(nlhs, plhs);

	if ( 1 < nlhs ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "Need one output arg");
		goto cleanup;
	}

	if ( 0 != nrhs ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "Expected no rhs argument");
		goto cleanup;
	}


	if ( ! (plhs[0] = mxCreateNumericMatrix( 1, 1, mxINT32_CLASS, mxREAL )) ) {
		mpvaSetError(&theErr, EZCA_FAILEDMALLOC, "Not enough memory");
		goto cleanup;
	}

	*(int*)mxGetData(plhs[0]) = ezcaGetRetryCount();

	nlhs = 0;

cleanup:
	ERR_CHECK(nlhs, plhs, &theErr);
}
