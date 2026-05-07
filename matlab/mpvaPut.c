
/* matlab wrapper for ezcaPut */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2002-2003  */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include "mglue.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
LcaError theErr;
int      onlhs = nlhs;

	mpvaMexGblInit();

	mpvaErrorInit(&theErr);

	LHSCHECK(nlhs, plhs);
	if ( 0 == onlhs )
		nlhs = 0;

	nlhs = theLcaPutMexFunction(nlhs,plhs,nrhs,prhs,1,&theErr);
	ERR_CHECK(nlhs, plhs, &theErr);
}
