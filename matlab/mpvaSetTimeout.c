
/* matlab wrapper for ezcaSetTimeout */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

#include <pvaClient.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
LcaError theErr;
float    timeout;

	mpvaMexGblInit();

	mpvaErrorInit(&theErr);

	LHSCHECK(nlhs, plhs);

	if ( 1 < nlhs ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "Need one output arg");
		goto cleanup;
	}

	if ( 1 != nrhs ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "Expected one rhs argument");
		goto cleanup;
	}

	if ( !mxIsDouble(prhs[0]) || 1 != mxGetM(prhs[0]) || 1 != mxGetN(prhs[0]) ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "Need a single numeric argument");
		goto cleanup;
	}

	timeout = mxGetScalar(prhs[0]);

	if ( timeout < 0.001 ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "Timeout arg must be >= 0.001");
		goto cleanup;
	}

	ezcaSetTimeout(timeout);

	nlhs = 0;

cleanup:
	ERR_CHECK(nlhs, plhs, &theErr);
}
