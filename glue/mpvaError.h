#ifndef MPVA_ERROR_H
#define MPVA_ERROR_H

/* mpva error interface */

#include <stdarg.h>
#include <shareLib.h>

#ifdef SCILAB_APP
#include <version.h>
#include <Scierror.h>
#include <api_scilab.h>
#endif

typedef struct LcaError_ {
	int  err;
	char msg[512];
	int  nerrs;  /* optional error vector */
	int  *errs;  /* optional error vector; must be FREEd when object goes out of scope */
} LcaError;

/* Errors returned by ezcaNewMonitorValue */

#define EZCA_NOMONITOR	20
#define EZCA_NOCHANNEL	21

#ifdef __cplusplus
extern "C" {
#endif

epicsShareFunc void epicsShareAPI
mpvaErrorInit(LcaError *pe);

/* remember last error that occurred;
 * this takes over an optional error vector
 * and NULLs pe->errs
 */
epicsShareFunc void epicsShareAPI
mpvaSaveLastError(LcaError *pe);

epicsShareFunc void epicsShareAPI
mpvaSetError(LcaError *pe, int err, const char *fmt, ...);

epicsShareFunc LcaError * epicsShareAPI
mpvaGetLastError();

#ifdef SCILAB_APP
#define MPVA_RAISE_ERROR(theError)	\
	do { \
		if ( (theError)->err ) { \
			mpvaSaveLastError(theError); \
			Scierror((theError)->err + 10000, (theError)->msg); \
		} else { \
		    ezcaFree((theError)->errs); (theError)->errs = 0; \
		    (theError)->nerrs = 0; \
		} \
	} while (0)

#define EZCA_SCIERR 100

int
mpvaCheckSciError(LcaError *pe, SciErr *sciErr);
#endif

#ifdef __cplusplus
};
#endif

#endif
