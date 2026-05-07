#include <api_scilab.h>
#include <mpvaSciApiHelpers.h>
#include <mpvaError.h>
#include <multiEzca.h>
#include <stdarg.h>
#include <stdio.h>
#include <pvaClient.h>

int
mpvaCheckSciError(LcaError *pe, SciErr *sciErr)
{
	if ( sciErr->iErr ) {
//		printError( sciErr, 0 );
		mpvaSetError(pe, EZCA_SCIERR, "%s", getErrorMessage( *sciErr ));
		return 1;
	}
	return 0;
}

static int
checkDim(LcaError *pe, int *mp, int m, int *np, int n)
{
	if ( *mp > 0 && (m != *mp) ) {
		mpvaSetError(pe, EZCA_INVALIDARG, "Invalid number of rows; %i expected, got %i\n", *mp, m);
		return 0;
	}

	*mp = m;

	if ( *np > 0 && (n != *np) ) {
		mpvaSetError(pe, EZCA_INVALIDARG, "Invalid number of columns; %i expected, got %i\n", *np, n);
		return 0;
	}

	*np = n;

	return 1;
}

char **
mpvaGetApiStringMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np)
{
int    m, n, i;
char  *dp;
int    dlen;
SciErr sciErr;
int   *pia  = 0;
int   *l    = 0;
char **p    = 0;
char **rval = 0;

	sciErr = getVarAddressFromPosition( pvApiCtx, idx, &pia );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		return 0;
	}

	sciErr = getMatrixOfString( pvApiCtx, pia, &m, &n, NULL, NULL );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		int type;
		mexPrintf("getMatrixOfString failed; deterining type");
		sciErr = getVarType( pvApiCtx, pia, &type );
		if ( sciErr.iErr ) {
			mexPrintf("yet ANOTER error\n");
		} else {
			mexPrintf("Type %i\n", type);
		}
		return 0;
	}

	if ( ! checkDim( pe, mp, m, np, n ) ) {
		return 0;
	}

	if ( ! (l = mpvaMalloc( sizeof(*l) * m * n )) ) {
		mpvaSetError(pe, EZCA_FAILEDMALLOC, "Allocating length array failed\n");
		return 0;
	}

	sciErr = getMatrixOfString( pvApiCtx, pia, &m, &n, l, NULL );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		goto bail;
	}

	dlen = 0;
	for ( i = 0; i < m*n; i++ ) {
		dlen += l[i] + 1;
	}

	if ( ! (p = mpvaMalloc( sizeof(*p) * m * n + sizeof(char)*dlen )) ) {
		mpvaSetError(pe, EZCA_FAILEDMALLOC, "Allocating data array failed\n");
		return 0;
	}

	dp = (char*) & p[m*n];
	for ( i = 0; i < m*n; i++ ) {
		p[i] = dp;
		dp  += l[i] + 1;
	}
	
	sciErr = getMatrixOfString( pvApiCtx, pia, &m, &n, l, p );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		goto bail;
	}

	rval = p;
	p    = 0;

bail:
	mpvaFree( l );
	mpvaFree( p );

	return rval;
}

int *
mpvaGetApiIntMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np)
{
int    m, n;
SciErr sciErr;
int   *pia  = 0;
int   *rval = 0;
int   *p    = 0;
	
	sciErr = getVarAddressFromPosition( pvApiCtx, idx, &pia );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		return 0;
	}

	sciErr = getMatrixOfInteger32( pvApiCtx, pia, &m, &n, &p );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		goto bail;
	}

	if ( ! checkDim( pe, mp, m, np, n ) ) {
		return 0;
	}

	rval = p;
	p    = 0;

bail:

	return rval;
}

double *
mpvaGetApiDblMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np)
{
int     m, n;
SciErr  sciErr;
int    *pia  = 0;
double *rval = 0;
double *p    = 0;
	
	sciErr = getVarAddressFromPosition( pvApiCtx, idx, &pia );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		return 0;
	}

	sciErr = getMatrixOfDouble( pvApiCtx, pia, &m, &n, &p );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		goto bail;
	}

	if ( ! checkDim( pe, mp, m, np, n ) ) {
		return 0;
	}

	rval = p;
	p    = 0;

bail:

	return rval;
}

void *
mpvaGetApiPtrMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np, int *type)
{
int    m, n;
SciErr sciErr;
int   *pia   = 0;
void  *pvPtr = 0;
void  *rval  = 0;

	sciErr = getVarAddressFromPosition( pvApiCtx, idx, &pia );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		return 0;
	}

	sciErr = getVarDimension( pvApiCtx, pia, &m, &n );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		return 0;
	}

	if ( ! checkDim( pe, mp, m, np, n ) ) {
		return 0;
	}

	sciErr = getPointer( pvApiCtx, pia, &pvPtr );
	if ( mpvaCheckSciError(pe, &sciErr) ) {
		goto bail;
	}

	if ( type ) {
		sciErr = getVarType( pvApiCtx, pia, type );
		if ( mpvaCheckSciError(pe, &sciErr) ) {
			goto bail;
		}
	}

	rval  = pvPtr;
	pvPtr = 0;

bail:

	return rval;
}

void
mpvaFreeApiStringMatrix(char **sm)
{
	mpvaFree( sm );
}
