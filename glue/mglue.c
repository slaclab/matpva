
/* MATLAB - EZCA interface glue utilites */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2003 */

/* LICENSE: EPICS open license, see ../LICENSE file */

#include <pvaClient.h>

#define epicsExportSharedSymbols
#include "shareLib.h"
#include "multiEzca.h"
#include "mglue.h"
#include "mpvaError.h"

void epicsShareAPI
releasePVs(PVs *pvs)
{
int i;
	if ( pvs && pvs->names ) {
		for ( i=0; i<pvs->m; i++ ) {
			mpvaFree(pvs->names[i]);
		}
		mpvaFree( pvs->names );
		multi_ezca_ctrlC_epilogue( &pvs->ctrlc );	
	}
}

int epicsShareAPI
buildPVs(const mxArray *pin, PVs *pvs, LcaError *pe)
{
char	**mem = 0;
size_t  i,m,buflen;
const mxArray *tmp;
int	rval = -1;

	if ( !pvs )
		return -1;

	pvs->names = 0;
	pvs->m     = 0;

	if ( !pin )
		return -1;

	if ( mxIsCell(pin) &&  1 != mxGetN(pin) ) {
			mpvaSetError(pe, EZCA_INVALIDARG, "Need a column vector of PV names\n");
			goto cleanup;
	}

	m = mxIsCell(pin) ? mxGetM(pin) : 1;

	if ( (! mxIsCell(pin) && ! mxIsChar(pin)) || m < 1 ) {
		mpvaSetError(pe, EZCA_INVALIDARG, "Need a (column) cell array argument with PV names");
		/* GENERAL CLEANUP NOTE: as far as I understand, this is not necessary:
		 *                       in mex files, garbage is automatically collected;
		 *                       explicit cleanup works in standalong apps also, however.
		 */
		goto cleanup;
	}

	if ( ! (mem = mpvaCalloc(m, sizeof(*mem))) ) {
		mpvaSetError(pe, EZCA_FAILEDMALLOC, "No Memory\n");
		goto cleanup;
	}


	for ( i=buflen=0; i<m; i++ ) {
		tmp = mxIsCell(pin) ? mxGetCell(pin, i) : pin;		
		if ( !tmp || !mxIsChar(tmp) || 1 != mxGetM(tmp) ) {
			mpvaSetError(pe, EZCA_INVALIDARG, "Not an vector of strings??");
			goto cleanup;
		}
		buflen = mxGetN(tmp) * sizeof(mxChar) + 1;
		if ( !(mem[i] = mpvaMalloc(buflen)) ) {
			mpvaSetError(pe, EZCA_FAILEDMALLOC, "No Memory\n");
			goto cleanup;
		}
		if ( mxGetString(tmp, mem[i], buflen) ) {
			mpvaSetError(pe, EZCA_INVALIDARG, "not a PV name?");
			goto cleanup;
		}
	}

	pvs->names = mem;
	pvs->m     = m;
	rval       = 0;

	multi_ezca_ctrlC_prologue(&pvs->ctrlc);

	pvs        = 0;

cleanup:
	releasePVs(pvs);
	return rval;	
}

const char * epicsShareAPI
mpvaErrorIdGet(int err)
{
	switch ( err ) {
		default: break;
		/* in case of EZCA_OK we really shouldn't get here... */
		case EZCA_OK:               return "mpva:unexpectedOK";
		case EZCA_INVALIDARG:       return "mpva:invalidArg";
		case EZCA_FAILEDMALLOC:     return "mpva:noMemory";
		case EZCA_CAFAILURE:        return "mpva:channelAccessFail";
		case EZCA_UDFREQ:           return "mpva:udfCaReq";
		case EZCA_NOTCONNECTED:     return "mpva:notConnected";
		case EZCA_NOTIMELYRESPONSE: return "mpva:timedOut";
		case EZCA_INGROUP:          return "mpva:inGroup";
		case EZCA_NOTINGROUP:       return "mpva:notInGroup";

		case EZCA_NOMONITOR:        return "mpva:noMonitor";
		case EZCA_NOCHANNEL:        return "mpva:noChannel";
		case EZCA_ABORTED:          return "mpva:usrAbort";
	}
	return "mpva:unkownError";
}

char epicsShareAPI
marg2ezcaType(const mxArray *typearg, LcaError *pe)
{
char typestr[2] = { 0 };

	if ( ! mxIsChar(typearg) ) {
		mpvaSetError(pe, EZCA_INVALIDARG, "(optional) type argument must be a string");
	} else {
		mxGetString( typearg, typestr, sizeof(typestr) );
		switch ( toupper(typestr[0]) ) {
			default:
 				break;

			case 'N':	return ezcaNative;
			case 'B':	return ezcaByte;
			case 'S':	return ezcaShort;
			case 'I':
			case 'L':	return ezcaLong;
			case 'F':	return ezcaFloat;
			case 'D':	return ezcaDouble;
			case 'C':	return ezcaString;
		}
	}
	mpvaSetError(pe, EZCA_INVALIDARG, "argument specifies an invalid data type");
	return ezcaInvalid;
}

int epicsShareAPI
flagError(int nlhs, mxArray *plhs[])
{
int i;
	if ( nlhs ) {
		for ( i=0; i<nlhs; i++ ) {
			mxDestroyArray(plhs[i]);
			/* hope this doesn't fail... */
			plhs[i] = 0;
		}
		return -1;
	}
	return 0;
}

int epicsShareAPI
theLcaPutMexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[], int doWait, LcaError *pe)
{
char	**pstr = 0;
size_t  i, m = 0, n = 0;
int		rval;
const	mxArray *tmp, *strval;
PVs     pvs = { {0}, };
char	type = ezcaNative;
mxArray *dummy = 0;
	
#ifdef LCAPUT_RETURNS_VALUE
	if ( nlhs == 0 )
		nlhs = 1;

	if ( nlhs > 1 ) {
		mpvaSetError(pe, EZCA_INVALIDARG, "Too many output args");
		goto cleanup;
	}
#else
	if ( nlhs ) {
		mpvaSetError(pe, EZCA_INVALIDARG, "Too many output args");
		goto cleanup;
	}
	nlhs = -1;
#endif

	if ( nrhs < 2 || nrhs > 3 ) {
		mpvaSetError(pe, EZCA_INVALIDARG, "Expected 2..3 rhs argument");
		goto cleanup;
	}

	n = mxGetN( tmp = prhs[1] );
	m = mxGetM( tmp );

	if ( mxIsChar( tmp ) ) {
		/* a single string; create a dummy cell matrix */
		m = n = 1;
		if ( !(dummy = mxCreateCellMatrix( m, n )) ) {
			mpvaSetError(pe, EZCA_FAILEDMALLOC, "Not Enough Memory");
			goto cleanup;
		}
		mxSetCell(dummy, 0, (mxArray*)mxDuplicateArray((mxArray*)tmp));
		tmp = dummy;
	}

	if ( mxIsCell( tmp ) ) {
		if ( !(pstr = mpvaCalloc( m * n, sizeof(*pstr) )) ) {
			mpvaSetError(pe, EZCA_FAILEDMALLOC, "Not Enough Memory");
			goto cleanup;
		}
		for ( i = 0; i < m * n; i++ ) {
			size_t len;
			if ( !mxIsChar( (strval = mxGetCell(tmp, i)) ) || 1 != mxGetM( strval ) ) {
				mpvaSetError(pe, EZCA_INVALIDARG, "Value argument must be a cell matrix of strings");
				goto cleanup;
			}
			len = mxGetN(strval) * sizeof(mxChar) + 1;
			if ( !(pstr[i] = mpvaMalloc(len)) ) {
				mpvaSetError(pe, EZCA_FAILEDMALLOC, "Not Enough Memory");
				goto cleanup;
			}
			if ( mxGetString(strval, pstr[i], len) ) {
				mpvaSetError(pe, EZCA_INVALIDARG, "Value still not a string (after all those checks) ???");
				goto cleanup;
			}
		}
		type = ezcaString;
	} else if ( ! mxIsDouble(tmp) ) {
			mpvaSetError(pe, EZCA_INVALIDARG, "2nd argument must be a double matrix");
			goto cleanup;
	} else {
		/* is a DOUBLE matrix */
	}

	if ( nrhs > 2 ) {
		char tmptype;
		if ( ezcaInvalid == (tmptype = marg2ezcaType(prhs[2], pe)) ) {
			goto cleanup;
		}
		if ( (ezcaString == type) != (ezcaString == tmptype) ) {
			mpvaSetError(pe, EZCA_UDFREQ, "string value type conversion not implemented, sorry");
			goto cleanup;
		}
		type = tmptype;
	}

	if ( buildPVs(prhs[0], &pvs, pe) )
		goto cleanup;

	assert( (pstr != 0) == (ezcaString ==  type) );

	rval = multi_ezca_put( pvs.names, (int)pvs.m, type, (pstr ? (void*)pstr : (void*)mxGetPr(prhs[1])), (int)m, (int)n, doWait, pe);

	if ( rval > 0 ) {
#ifdef LCAPUT_RETURNS_VALUE
		if ( !(plhs[0] = mxCreateDoubleMatrix(1,1,mxREAL)) ) {
			mpvaSetError(pe, EZCA_FAILEDMALLOC, "No Memory\n");
			goto cleanup;
		}
		*mxGetPr(plhs[0]) = (double)rval;
#endif
		nlhs = 0;
	}

cleanup:
	if ( pstr ) {
		/* free string elements also */
		for ( i=0; i<m*n; i++ ) {
			mpvaFree( pstr[i] );
		}
		mpvaFree( pstr );
	}
	if ( dummy ) {
		mxDestroyArray( dummy );
	}
	releasePVs(&pvs);
	return nlhs;
}
