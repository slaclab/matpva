#ifndef  MATLAB_EZCA_GLUE_H

/* matlab-ezca interface utility header */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2003 */

/* LICENSE: EPICS open license, see ../LICENSE file */
#include <mex.h>
#include <matrix.h>
#include <multiEzcaCtrlC.h>
#include <mpvaError.h>
#include <pvaClient.h>

typedef struct PVs_ {
	CtrlCStateRec	ctrlc;
	size_t    		m;
	char 			**names;
} PVs;

#include "shareLib.h"

#ifdef __cplusplus
extern "C" {
#endif

epicsShareFunc void epicsShareAPI
releasePVs(PVs *pvs);

epicsShareFunc int epicsShareAPI
buildPVs(const mxArray *pin, PVs *pvs, LcaError *pe);

epicsShareFunc const char * epicsShareAPI
mpvaErrorIdGet(int err);

/* check for 'typearg' being a string and use the
 * first character to determine the desired ezca type
 *  ezca'N'ative
 *  ezca'B'yte
 *  ezca'S'hort
 *  ezca'L'ong
 *  ezca'F'loat
 *  ezca'D'ouble
 * or 'C'har for ezcaString
 */
epicsShareFunc char epicsShareAPI
marg2ezcaType(const mxArray *typearg, LcaError *pe);

/* use 'nlhs' as an 'error' flag; (jumped out of something and
 * have already assigned 'lhs' args).
 * Clean up the lhs args and flag an error condition.
 * If everyhing's OK, the caller passes nlhs == 0.
 */
epicsShareFunc int epicsShareAPI
flagError(int nlhs, mxArray *plhs[]);

/* assert that 'nlhs' >= 1 and clear *plhs */
#define LHSCHECK(nlhs, plhs)           \
	do {                           \
		int xxx; \
		if ( nlhs < 1 ) {      \
			nlhs = 1;      \
		}                      \
		for ( xxx = 0; xxx < nlhs; xxx++ ) { \
			plhs[xxx] = 0; \
		}                      \
	} while (0)

/* mexErrMsgTxt() should be called only from a routine
 * that was compiled with the mex compiler in order to
 * reduce C++ problems (mexErrMsgTxt throws a C++ exception).
 * For this reason we define a macro.
 */
#define ERR_CHECK(nlhs, plhs, perr) \
	do { if (flagError((nlhs),(plhs))) { \
			mpvaSaveLastError(perr); \
			if ( (perr)->errs ) \
				ezcaFree( (perr)->errs ); \
			mexErrMsgIdAndTxt(mpvaErrorIdGet((perr)->err), (perr)->msg); \
		 } else { \
		 	ezcaFree((perr)->errs); (perr)->errs = 0; \
			(perr)->nerrs = 0; \
		 } \
	} while (0)
epicsShareFunc int epicsShareAPI
theLcaPutMexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[], int doWait, LcaError *pe);

/*
 * Global initialization that *must* be performed from MEX-file context
 */
epicsShareFunc void
mpvaMexGblInit();

#ifdef __cplusplus
};
#endif

#endif
