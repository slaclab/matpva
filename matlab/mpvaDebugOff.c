
/* matlab wrapper for ezcaDebugOff() */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"
#include "multiEzca.h"

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

	ezcaDebugOff();

	nlhs = 0;

cleanup:
	ERR_CHECK(nlhs, plhs, &theErr);
}
