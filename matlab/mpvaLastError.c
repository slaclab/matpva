
/* matlab wrapper for mpvaGetLastError */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2007 */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

#include <pvaClient.h>

#include <ctype.h>

#include <epicsTypes.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
int      i;
int      *src;
int      *dst;
LcaError *ptheErr = mpvaGetLastError();
LcaError theErr;

	mpvaMexGblInit();

	mpvaErrorInit(&theErr);

	LHSCHECK(nlhs, plhs);

	if ( nlhs > 1 ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "Too many output args");
		goto cleanup;
	}

	if ( nrhs ) {
		mpvaSetError(&theErr, EZCA_INVALIDARG, "Expected no rhs argument");
		goto cleanup;
	}

	if ( (i=ptheErr->nerrs) ) {
		src = ptheErr->errs;
	} else {
		i   = 1;
		src = &ptheErr->err;
	}

	if ( ! (plhs[0] = mxCreateNumericMatrix( i, 1, mxINT32_CLASS, mxREAL )) ) {
		mpvaSetError(&theErr, EZCA_FAILEDMALLOC, "Not enough memory");
		goto cleanup;
	}

	dst = (int*)mxGetData(plhs[0]);

	while ( i-- ) {
		*dst++ = *src++;
	}

	nlhs = 0;

cleanup:

	/* do this LAST (in case mexErrMsgTxt is called) */
	ERR_CHECK(nlhs, plhs, &theErr);
}
