#ifndef SCILAB_GATEWAY_WRAPPER_H
#define SCILAB_GATEWAY_WRAPPER_H

#include <mpvaSciApiHelpers.h>
#include <sciclean.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

int intsezcaGet(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaPut(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaPutNoWait(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetNelem(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetControlLimits(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetGraphicLimits(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetWarnLimits(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetAlarmLimits(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetStatus(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetPrecision(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetUnits(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetEnumStrings(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetRetryCount(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaSetRetryCount(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaGetTimeout(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaSetTimeout(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaDebugOn(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaDebugOff(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaSetSeverityWarnLevel(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaClearChannels(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaSetMonitor(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaNewMonitorValue(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaNewMonitorWait(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaDelay(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);
int intsezcaLastError(char *name, PvApiCtxType pvApiCtx, Sciclean cleanup);

int
MPVA(wchar_t *funcName);

#ifdef __cplusplus
};
#endif

#endif
