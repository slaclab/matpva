#ifndef MPVA_SCI_API_HELPERS_H
#define MPVA_SCI_API_HELPERS_H

#include <api_scilab.h>
#include <mpvaError.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *PvApiCtxType;

int
mpvaCheckSciError(LcaError *pe, SciErr *sciErr);

char **
mpvaGetApiStringMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np);

void *
mpvaGetApiPtrMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np, int *type);

int *
mpvaGetApiIntMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np);

double *
mpvaGetApiDblMatrix(PvApiCtxType pvApiCtx, LcaError *pe, int idx, int *mp, int *np);

void
mpvaFreeApiStringMatrix(char **sm);

#ifdef __cplusplus
};
#endif

#endif
