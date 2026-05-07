
/* SCILAB C-interface to ezca / multiEzca */
#include <version.h>
#include <api_scilab.h>
#include <sciprint.h>
#include <string.h>
#include <pvaClient.h>
#include <mpvaSciApiHelpers.h>

#include <sciclean.h>

#define epicsExportSharedSymbols
#include <shareLib.h>

#include <multiEzca.h>
#include <multiEzcaCtrlC.h>

/* Note about cleanup and errors:
 *
 * A lot of scilab macros (e.g., CreateVar()) expand
 * to a check and return immediately from the current
 * subroutine
 *
 *    if ( !C2F(createvar)() ) return 0;
 *
 * so that we don't get a chance to cleanup resources
 * that we allocate.
 *
 * We employ the 'sciclean' facility to work around
 * this problem.
 */

/* Define a macro to cleanup memory obtained
 * via 'mpvaMalloc' & friends.
 */

#define MPVACLEAN(var) sciclean_push(sciclean, (var), mpvaFree)
static void
mpva_free_strings(void *x)
{
char **strs = x;
	if ( strs ) {
		while ( *strs ) {
			mpvaFree(*strs);
			strs++;
		}
		mpvaFree(x);
	}
}
#define MPVACLEAN_SVAR(var) sciclean_push(sciclean, (var), mpva_free_strings)
#define SCICLEAN_SVAR(var) SCICLEAN_CLNF( (var), (void (*)(void*))mpvaFreeApiStringMatrix )

/* We get a lot of 'type-punned' pointer warnings, mostly
 * because the scilab header doesn't properly declare things
 * (using integer* or double* where a void* would be more
 * appropriate). In fact, we can probably expect that the
 * pointers in question are eventually accessing data objects
 * of the correct type.
 * Nevertheless, in order to silence warnings (and avoid possible
 * problems) we declare all such pointers with the attribute
 * 'may_alias' [gnu only].
 */
#ifdef __GNUC__
#define MAY_ALIAS __attribute__((may_alias))
#else
#define MAY_ALIAS
#endif

static int
arg2ezcaType(char *pt, int idx, LcaError *pe, PvApiCtxType pvApiCtx)
{
int    m,n;
char **strs;
char   ch;

	if ( Rhs < idx )
		return 0;

	m = n = 1;
	if ( ! (strs = mpvaGetApiStringMatrix(pvApiCtx, pe, idx, &m, &n)) ) {
		return 0;
	}

	ch = toupper( strs[0][0] );

	mpvaFreeApiStringMatrix( strs );
	
	switch ( ch ) {
			case 'D': *pt = ezcaDouble; break;
			case 'F': *pt = ezcaFloat;  break;
			case 'L': *pt = ezcaLong;   break;
			case 'S': *pt = ezcaShort;  break;
			case 'B': *pt = ezcaByte;   break;
			case 'C': *pt = ezcaString; break;
			case 'N': *pt = ezcaNative; break;
                                                                                            
			default:
					mpvaSetError(pe, EZCA_INVALIDARG, "Invalid type - must be 'byte', 'short', 'long', 'float', 'double' or 'char'");
			return 0;
	}

	return 1;
}

static void errRaiseClean(void *obj)
{
LcaError *theErr = obj;
	MPVA_RAISE_ERROR(theErr);
	mpvaFree( theErr );
}

static LcaError *errCreate(Sciclean sciclean)
{
LcaError *theErr = mpvaMalloc(sizeof(*theErr));
	mpvaErrorInit(theErr);
	sciclean_push(sciclean, theErr, errRaiseClean);
	return theErr;
}

int intsezcaGet(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int             mpvs, mtmp, ntmp, status, itmp;
double         *reptr = 0, *imptr = 0;
char          **pvs MAY_ALIAS = 0;
void           *buf MAY_ALIAS = 0;
int	            n             = 0;
double         *dptr;
char            type          = ezcaNative;
epicsTimeStamp *ts            = 0;
LcaError       *theErr        = errCreate(sciclean);
SciErr          sciErr;

	CheckInputArgument(pvApiCtx,1,3);
	CheckOutputArgument(pvApiCtx,0,2);

	mpvs = -1; ntmp = 1;
	pvs  = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &mpvs, &ntmp);
	if ( ! pvs ) {
		goto bail;
	}
	SCICLEAN_SVAR(pvs);

	if ( Rhs > 1 ) {
		mtmp = ntmp = 1;
		if ( ! (dptr = mpvaGetApiDblMatrix( pvApiCtx, theErr, 2, &mtmp, &ntmp )) ) {
			goto bail;
		}
		n = (int) round(*dptr);
		if ( Rhs > 2 && !arg2ezcaType(&type,3, theErr, pvApiCtx) )
			goto bail;
	}

	if ( Lhs >= 2 ) {
		ntmp = 1;
		sciErr = allocComplexMatrixOfDouble( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, mpvs, ntmp, &reptr, &imptr );
		if ( mpvaCheckSciError(theErr, &sciErr) ) {
			goto bail;
		}
	}

	/* we can't preallocate the result matrix -- n is unknown at this point */
	status = multi_ezca_get( pvs, &type, &buf, mpvs, &n, &ts, theErr );

	/* register cleanups for memory allocated by multi_ezca_get */
	MPVACLEAN(ts);
	if ( ezcaString == type ) {
		MPVACLEAN_SVAR(buf);
	} else {
		MPVACLEAN(buf);
	}

	if ( !status ) {
		for ( itmp = 1; itmp <= Lhs; itmp++ ) {
    		AssignOutputVariable(pvApiCtx, itmp) = 0;
		}
		goto bail;
	}

	if ( Lhs >= 0 ) {
		if ( ezcaString == type ) {
			sciErr = createMatrixOfString( pvApiCtx, nbInputArgument( pvApiCtx ) + 2, mpvs, n, (const char * const *)buf );
		} else {
			sciErr = createMatrixOfDouble( pvApiCtx, nbInputArgument( pvApiCtx ) + 2, mpvs, n, (double*)buf );
		}
		if ( mpvaCheckSciError(theErr, &sciErr) ) {
			goto bail;
		}
    	AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 2;

		if ( Lhs >= 2 ) {
			multi_ezca_ts_cvt( mpvs, ts, reptr, imptr );
    		AssignOutputVariable(pvApiCtx, 2) = nbInputArgument( pvApiCtx ) + 1;
		}
	}

bail:

	return 0;
}

static int dosezcaPut(char *fname, int doWait, Sciclean sciclean, PvApiCtxType pvApiCtx)
{
int       mpvs, mval, ntmp, n;
void     *buf MAY_ALIAS;
char    **pvs MAY_ALIAS;
char      type   = ezcaNative;
LcaError *theErr = errCreate(sciclean);
SciErr    sciErr;
int      *pia;
int       sciType;

	CheckInputArgument(pvApiCtx,2,3);
	CheckOutputArgument(pvApiCtx,0,
#ifdef LCAPUT_RETURNS_VALUE
		1
#else
		0
#endif
	);

	mpvs = -1;
	ntmp =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &mpvs, &ntmp)) ) {
		return 0;
	}
	SCICLEAN_SVAR(pvs);

	sciErr = getVarAddressFromPosition( pvApiCtx, 2, &pia );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}
	
	sciErr = getVarType( pvApiCtx, pia, &sciType );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}

	mval = n = -1;
	if ( sci_strings == sciType ) {
		if ( ! (buf = (void*)mpvaGetApiStringMatrix(pvApiCtx, theErr, 2, &mval, &n)) ) {
			return 0;
		}
		SCICLEAN_SVAR(buf);
		type = ezcaString;
	} else {
		if ( ! (buf = (void*)mpvaGetApiDblMatrix(pvApiCtx, theErr, 2, &mval, &n)) ) {
			return 0;
		}
	}

	if ( Rhs > 2 ) {
		char t;
		if ( !arg2ezcaType( &t, 3, theErr , pvApiCtx) )
			goto bail;
		if ( (ezcaString == type) != (ezcaString == t) )  {
			mpvaSetError(theErr, EZCA_INVALIDARG, "string value type conversion not implemented, sorry");
			goto bail;
		}
		type = t;
	}

#ifdef LCAPUT_RETURNS_VALUE
	{
	double *dptr;
	CreateVar(4,"r",&one,&one,&i);
		sciErr = allocMatrixOfDouble( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, 1, 1, &dptr );
		if ( mpvaCheckSciError(theErr, &sciErr) ) {
			return 0;
		}
    	AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;
	*dptr =
#endif

		multi_ezca_put(pvs, mpvs, type, buf, mval, n, doWait, theErr);

#ifdef LCAPUT_RETURNS_VALUE
	}
#endif

bail:
	return 0;
}

int intsezcaPutNoWait(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
	return dosezcaPut(fname, 0, sciclean, pvApiCtx);
}

int intsezcaPut(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
	return dosezcaPut(fname, 1, sciclean, pvApiCtx);
}


int intsezcaGetNelem(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int       m,n,*iptr;
char    **pvs MAY_ALIAS;
LcaError *theErr = errCreate(sciclean);
SciErr    sciErr;

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,1);

	m = -1;
	n =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}
	SCICLEAN_SVAR(pvs);

	sciErr = allocMatrixOfInteger32( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, m, n, &iptr );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}

	if ( !multi_ezca_get_nelem(pvs, m, iptr, theErr) ) {
    	AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;
	}

 	return 0;
}

static int getLimits(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean, MultiEzcaFunc f)
{
int         m,n;
double     *dptr1,*dptr2;
char      **pvs MAY_ALIAS;
MultiArgRec args[2];
LcaError   *theErr = errCreate(sciclean);
SciErr      sciErr;
int         numLhsVars;

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,2);

	m = -1;
	n =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}
	SCICLEAN_SVAR(pvs);

	sciErr = allocMatrixOfDouble( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, m, n, &dptr1 );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}

	sciErr = allocMatrixOfDouble( pvApiCtx, nbInputArgument( pvApiCtx ) + 2, m, n, &dptr2 );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}

	MSetArg(args[0], sizeof(double), dptr1, 0); 
	MSetArg(args[1], sizeof(double), dptr2, 0); 

	numLhsVars = Lhs > 0 ? Lhs : 1;

	if ( multi_ezca_get_misc(pvs, m, f, NumberOf(args), args, theErr) ) {
		for ( n=1; n<=numLhsVars; n++ ) {
	    	AssignOutputVariable(pvApiCtx, n) = nbInputArgument( pvApiCtx ) + n;
		}
	}
	
	return 0;
}

int intsezcaGetControlLimits(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
	return getLimits(fname, pvApiCtx, sciclean, (MultiEzcaFunc)ezcaGetControlLimits);
}

int intsezcaGetGraphicLimits(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
	return getLimits(fname, pvApiCtx, sciclean, (MultiEzcaFunc)ezcaGetGraphicLimits);
}

int intsezcaGetWarnLimits(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
	return getLimits(fname, pvApiCtx, sciclean, (MultiEzcaFunc)ezcaGetWarnLimits);
}

int intsezcaGetAlarmLimits(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
	return getLimits(fname, pvApiCtx, sciclean, (MultiEzcaFunc)ezcaGetAlarmLimits);
}

int intsezcaGetStatus(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
epicsTimeStamp *ts MAY_ALIAS = 0;
int            m,n, status;
char           **pvs MAY_ALIAS;
LcaError       *theErr = errCreate(sciclean);
short          *sptr;
double         *reptr = 0, *imptr = 0;
SciErr          sciErr;
MultiArgRec     args[3];

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,3);

	m = -1;
	n =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}

	SCICLEAN_SVAR(pvs);

	MSetArg(args[0], sizeof(epicsTimeStamp), 0,       &ts); /* timestamp */

	sciErr = allocMatrixOfInteger16( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, m, n, &sptr );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}
	MSetArg(args[1], sizeof(short),    sptr, 0  ); /* status    */

	sciErr = allocMatrixOfInteger16( pvApiCtx, nbInputArgument( pvApiCtx ) + 2, m, n, &sptr );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}
	MSetArg(args[2], sizeof(short),    sptr, 0  ); /* severity  */

	status = multi_ezca_get_misc(pvs, m, (MultiEzcaFunc)ezcaGetStatus, NumberOf(args), args, theErr);

	/* If ts was created then register a cleanup */
	MPVACLEAN(ts);

	if ( status ) {

	    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 2;

		if ( Lhs >= 2 ) {
	    	AssignOutputVariable(pvApiCtx, 2) = nbInputArgument( pvApiCtx ) + 1;
			
			if ( Lhs >= 3 ) {
				sciErr = allocComplexMatrixOfDouble( pvApiCtx, nbInputArgument( pvApiCtx ) + 3, m, n, &reptr, &imptr );
				if ( mpvaCheckSciError(theErr, &sciErr) ) {
					return 0;
				}
				multi_ezca_ts_cvt( m, ts, reptr, imptr );
	    		AssignOutputVariable(pvApiCtx, 3) = nbInputArgument( pvApiCtx ) + 3;
			}
		}
	}

 	return 0;
}

int intsezcaGetPrecision(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int         m,n;
char      **pvs MAY_ALIAS;
MultiArgRec	args[1];
LcaError   *theErr = errCreate(sciclean);
SciErr      sciErr;
short      *sptr;

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,1);

	m    = -1;
	n    =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}

	SCICLEAN_SVAR(pvs);

	sciErr = allocMatrixOfInteger16( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, m, n, &sptr );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}

	MSetArg(args[0], sizeof(short), sptr, 0);

	if ( multi_ezca_get_misc(pvs, m, (MultiEzcaFunc)ezcaGetPrecision, NumberOf(args), args, theErr) ) {
	    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;
	}
 	return 0;
}

typedef char units_string[EZCA_UNITS_SIZE];

int intsezcaGetUnits(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int            m,n,i,status;
char         **pvs MAY_ALIAS,**tmp;
units_string  *strbuf MAY_ALIAS = 0;
MultiArgRec    args[1];
LcaError      *theErr = errCreate(sciclean);
SciErr         sciErr;

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,1);

	m = -1;
	n =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}
	SCICLEAN_SVAR(pvs);

	MSetArg(args[0], sizeof(units_string), 0, &strbuf);

	status = multi_ezca_get_misc(pvs, m, (MultiEzcaFunc)ezcaGetUnits, NumberOf(args), args, theErr);

	/* if strbuf was created register a cleanup */
	MPVACLEAN(strbuf);

	if ( status && 
		 /* scilab expects NULL terminated char** list */
		 (tmp = mpvaMalloc(sizeof(char*) * (m+1))) ) {
		for ( i=0; i<m; i++ ) {
			tmp[i] = strbuf[i];
		}
		tmp[m] = 0;
		MPVACLEAN(tmp); /* register cleanup; CreateVarFromPtr may fail and execute 'return 0' */

		sciErr = createMatrixOfString( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, m, n, (const char * const *)tmp );
		if ( mpvaCheckSciError(theErr, &sciErr) ) {
			return 0;
		}
		AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;
	}

 	return 0;
}

typedef char enum_strings[EZCA_ENUM_STATES][EZCA_ENUM_STRING_SIZE];

int intsezcaGetEnumStrings(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int           m,n,i,j,ij,status;
char        **pvs MAY_ALIAS,**tmp;
enum_strings *strbuf MAY_ALIAS = 0;
MultiArgRec   args[1];
LcaError     *theErr = errCreate(sciclean);
SciErr        sciErr;

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,1);

	m = -1;
	n =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}
	SCICLEAN_SVAR(pvs);

	MSetArg(args[0], sizeof(enum_strings), 0, &strbuf);

	n     = EZCA_ENUM_STATES;
	status = multi_ezca_get_misc(pvs, m, (MultiEzcaFunc)ezcaGetEnumStrings, NumberOf(args), args, theErr);

	/* if strbuf was created register a cleanup */
	MPVACLEAN(strbuf);

	if ( status && 
		 /* scilab expects NULL terminated char** list */
		 (tmp = mpvaMalloc(sizeof(char*) * (n*m+1))) ) {
		ij = 0;
		for ( j=0; j<n; j++ ) {
			for ( i=0; i<m; i++ ) {
				tmp[ij++] = strbuf[i][j];
			}
		}
		tmp[ij] = 0;
		MPVACLEAN(tmp); /* register cleanup; CreateVarFromPtr may fail and execute 'return 0' */

		sciErr = createMatrixOfString( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, m, n, (const char * const *)tmp );
		if ( mpvaCheckSciError(theErr, &sciErr) ) {
			return 0;
		}
		AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;
	}

 	return 0;
}


int intsezcaGetRetryCount(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int       m=1,*iptr;
SciErr    sciErr;
LcaError *theErr = errCreate(sciclean);

	CheckInputArgument(pvApiCtx,0,0);
	CheckOutputArgument(pvApiCtx,0,1);

	sciErr = allocMatrixOfInteger32( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, m, m, &iptr );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}
	*iptr = ezcaGetRetryCount();
	AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;
	return 0;
}

int intsezcaSetRetryCount(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int       m,n,cnt;
double   *dptr;
LcaError *theErr = errCreate(sciclean);

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,1);

	m = n = 1;
	if ( ! (dptr = mpvaGetApiDblMatrix( pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}

	cnt = (int)round(*dptr);

	if ( cnt < 1 ) {
		mpvaSetError(theErr, EZCA_INVALIDARG, "mpvaSetRetryCount(): arg >= 1 expected");
		goto cleanup;
	}
	ezcaSetRetryCount( cnt );

cleanup:
	AssignOutputVariable(pvApiCtx, 1) = 0;
	return 0;
}

int intsezcaGetTimeout(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
double   *dptr;
SciErr    sciErr;
LcaError *theErr = errCreate(sciclean);


	CheckInputArgument(pvApiCtx,0,0);
	CheckOutputArgument(pvApiCtx,0,1);

	sciErr = allocMatrixOfDouble( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, 1, 1, &dptr );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		return 0;
	}

	*dptr = ezcaGetTimeout();
	AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;
	return 0;
}

int intsezcaSetTimeout(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int       m,n;
double   *dptr;
LcaError *theErr = errCreate(sciclean);
float     timeout;

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,1);

	m = n = 1;
	if ( ! (dptr = mpvaGetApiDblMatrix( pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}

	timeout = (float)*dptr;

	if ( timeout < 0.001 ) {
		mpvaSetError(theErr, EZCA_INVALIDARG, "Timeout arg must be >= 0.001");
		goto cleanup;
	}
	ezcaSetTimeout(timeout);

cleanup:
	AssignOutputVariable(pvApiCtx, 1) = 0;
	return 0;
}

int intsezcaDelay(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int      m,n,rc;
double   *dptr;
LcaError *theErr = errCreate(sciclean);

	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,1);

	m = n = 1;
	if ( ! (dptr = mpvaGetApiDblMatrix( pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}

	if ( (rc = ezcaDelay(*dptr)) ) {
		char *msg="mpvaDelay";
		LcaError *theErr = errCreate(sciclean);
		switch ( rc ) {
			case EZCA_INVALIDARG: msg = "mpvaDelay: need 1 timeout arg > 0";
			break;
			case EZCA_ABORTED:    msg = "mpvaDelay: usr abort";
			break;
			default:
			break;
		}
		mpvaSetError(theErr, rc, msg);
	}
	AssignOutputVariable(pvApiCtx, 1) = 0;
	return 0;
}


int intsezcaLastError(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int m, ntmp, *i, *src;
LcaError *ple = mpvaGetLastError();
SciErr    sciErr;

	CheckInputArgument(pvApiCtx,0,0);
	CheckOutputArgument(pvApiCtx,0,1);

	ntmp = 1;

	if ( ! (src = ple->errs) ) {
		/* single error */
		m   = 1;
		src = & ple->err;
	} else {
		m   = ple->nerrs;
	}

	sciErr = allocMatrixOfInteger32( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, m, ntmp, &i );
	if ( mpvaCheckSciError(ple, &sciErr) ) {
		return 0;
	}
 	
	if ( sizeof(*i) != sizeof(*src) ) {
		sciprint("FATAL ERROR -- type size mismatch: %s - %i",__FILE__, __LINE__);
	}
	memcpy(i, src, m*sizeof(*i));

	AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;

	return 0;
}

int intsezcaDebugOn(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
	CheckInputArgument(pvApiCtx,0,0);
	CheckOutputArgument(pvApiCtx,0,1);
	ezcaDebugOn();
	AssignOutputVariable(pvApiCtx, 1) = 0;
	return 0;
}

int intsezcaDebugOff(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
	CheckInputArgument(pvApiCtx,0,0);
	CheckOutputArgument(pvApiCtx,0,1);
	ezcaDebugOff();
	AssignOutputVariable(pvApiCtx, 1) = 0;
	return 0;
}

int intsezcaClearChannels(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int m,n;
char **s MAY_ALIAS;
LcaError *theErr = errCreate(sciclean);

	CheckInputArgument(pvApiCtx,0,1);
	CheckOutputArgument(pvApiCtx,0,1);
	if ( Rhs > 0 ) {

		m = -1;
		n =  1;
		if ( ! (s = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &m, &n)) ) {
			return 0;
		}
		SCICLEAN_SVAR(s);

		if ( 1 == m && 0 == *s[0] ) {
			s = 0;
			m = 0;
		}
	} else {
		s = 0;
		m = -1;
	}

	if ( multi_ezca_clear_channels(s,m,theErr) ) {
		/* error */
		return 0;
	}
		
	return 0;
}

int intsezcaSetSeverityWarnLevel(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int       m,n;
double   *dptr;
LcaError *theErr = errCreate(sciclean);
	CheckInputArgument(pvApiCtx,1,1);
	CheckOutputArgument(pvApiCtx,0,1);

	m = n = 1;
	if ( ! (dptr = mpvaGetApiDblMatrix( pvApiCtx, theErr, 1, &m, &n)) ) {
		return 0;
	}

	ezcaSetSeverityWarnLevel((int)round(*dptr));
	AssignOutputVariable(pvApiCtx, 1) = 0;
	return 0;
}

int intsezcaSetMonitor(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int       mpvs, mtmp, ntmp, n = 0;
double   *dptr;
char    **pvs MAY_ALIAS;
char      type  = ezcaNative;
LcaError *theErr = errCreate(sciclean);

	CheckInputArgument(pvApiCtx,1,3);
	CheckOutputArgument(pvApiCtx,0,1);

	mpvs = -1;
	ntmp =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &mpvs, &ntmp)) ) {
		return 0;
	}
	SCICLEAN_SVAR(pvs);

	if ( Rhs > 1 ) {
		mtmp = ntmp = 1;
		if ( ! (dptr = mpvaGetApiDblMatrix( pvApiCtx, theErr, 2, &mtmp, &ntmp )) ) {
			goto cleanup;
		}
		n = (int)round(*dptr);
		if ( Rhs > 2 && !arg2ezcaType(&type,3, theErr, pvApiCtx) )
			goto cleanup;
	}

	(void) multi_ezca_set_mon(pvs, mpvs, type, n, theErr);

cleanup:
	return 0;
}

int intsezcaNewMonitorValue(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int mpvs, ntmp, *i;
char     **pvs MAY_ALIAS;
char      type  = ezcaNative;
LcaError *theErr = errCreate(sciclean);
SciErr    sciErr;

	CheckInputArgument(pvApiCtx,1,2);
	CheckOutputArgument(pvApiCtx,0,1);

	mpvs = -1;
	ntmp =  1;
	if ( ! (pvs = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &mpvs, &ntmp)) ) {
		return 0;
	}
	SCICLEAN_SVAR(pvs);

	if ( Rhs > 1 && !arg2ezcaType(&type,2,theErr,pvApiCtx) )
		goto cleanup;

	ntmp = 1;
	sciErr = allocMatrixOfInteger32( pvApiCtx, nbInputArgument( pvApiCtx ) + 1, mpvs, ntmp, &i );
	if ( mpvaCheckSciError(theErr, &sciErr) ) {
		goto cleanup;
	}
 		
	(void) multi_ezca_check_mon(pvs, mpvs, type, i, theErr);

	AssignOutputVariable(pvApiCtx, 1) = nbInputArgument( pvApiCtx ) + 1;

cleanup:
	return 0;
}

int intsezcaNewMonitorWait(char *fname, PvApiCtxType pvApiCtx, Sciclean sciclean)
{
int mpvs, ntmp;
char     **pvs MAY_ALIAS;
char      type  = ezcaNative;
LcaError *theErr = errCreate(sciclean);

	CheckInputArgument(pvApiCtx,1,2);
	CheckOutputArgument(pvApiCtx,0,1);

	mpvs = -1; ntmp = 1;
	pvs  = mpvaGetApiStringMatrix(pvApiCtx, theErr, 1, &mpvs, &ntmp);
	if ( ! pvs ) {
		goto cleanup;
	}
	SCICLEAN_SVAR(pvs);

	if ( Rhs > 1 && !arg2ezcaType(&type,2,theErr, pvApiCtx) )
		goto cleanup;

	(void) multi_ezca_wait_mon(pvs, mpvs, type, theErr);

cleanup:
	return 0;
}
