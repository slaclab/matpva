#include <scilabGateway.h>
#include <sciclean.h>
#include <multiEzcaCtrlC.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <c_gateway_prototype.h>
#include <addfunction.h>
#ifdef __cplusplus
}
#endif

static int
mpva_trampoline(char *fname, PvApiCtxType pvApiCtx, ScicleanGatefunc gate)
{
int           rval;
CtrlCStateRec save;

	multi_ezca_ctrlC_prologue(&save);

	rval = sciclean_trampoline(fname, pvApiCtx, gate);

	multi_ezca_ctrlC_epilogue(&save);

	return rval;
}

template <ScicleanGatefunc gate> int
mpva_gateway(char *fname, void *uarg)
{
	return mpva_trampoline(fname, uarg, gate);
}

typedef int (*Myinterfun)(char*, PvApiCtxType );

struct { Myinterfun f ;  const wchar_t *name; } Tab[]={
	{mpva_gateway<intsezcaGet>,					L"mpvaGet"},
	{mpva_gateway<intsezcaPut>,					L"mpvaPut"},
	{mpva_gateway<intsezcaPutNoWait>,				L"mpvaPutNoWait"},
	{mpva_gateway<intsezcaGetNelem>,				L"mpvaGetNelem"},
	{mpva_gateway<intsezcaGetControlLimits>,		L"mpvaGetControlLimits"},
	{mpva_gateway<intsezcaGetGraphicLimits>,		L"mpvaGetGraphicLimits"},
	{mpva_gateway<intsezcaGetWarnLimits>,		    L"mpvaGetWarnLimits"},
	{mpva_gateway<intsezcaGetAlarmLimits>,			L"mpvaGetAlarmLimits"},
	{mpva_gateway<intsezcaGetStatus>,				L"mpvaGetStatus"},
	{mpva_gateway<intsezcaGetPrecision>,			L"mpvaGetPrecision"},
	{mpva_gateway<intsezcaGetUnits>,				L"mpvaGetUnits"},
	{mpva_gateway<intsezcaGetEnumStrings>,			L"mpvaGetEnumStrings"},
	{mpva_gateway<intsezcaGetRetryCount>,			L"mpvaGetRetryCount"},
	{mpva_gateway<intsezcaSetRetryCount>,			L"mpvaSetRetryCount"},
	{mpva_gateway<intsezcaGetTimeout>,				L"mpvaGetTimeout"},
	{mpva_gateway<intsezcaSetTimeout>,				L"mpvaSetTimeout"},
	{mpva_gateway<intsezcaDebugOn>,				L"mpvaDebugOn"},
	{mpva_gateway<intsezcaDebugOff>,				L"mpvaDebugOff"},
	{mpva_gateway<intsezcaSetSeverityWarnLevel>,	L"mpvaSetSeverityWarnLevel"},
	{mpva_gateway<intsezcaClearChannels>,			L"mpvaClear"},
	{mpva_gateway<intsezcaSetMonitor>,				L"mpvaSetMonitor"},
	{mpva_gateway<intsezcaNewMonitorValue>,	    L"mpvaNewMonitorValue"},
	{mpva_gateway<intsezcaNewMonitorWait>,			L"mpvaNewMonitorWait"},
	{mpva_gateway<intsezcaDelay>,	    			L"mpvaDelay"},
	{mpva_gateway<intsezcaLastError>,    			L"mpvaLastError"},
};

/* WIN:
 * epicsShareAPI causes leading underscore which is not expected by 
 * scilab 'addinter()'.  However, if a function has varargs then 
 * the non __stdcall API seems to be enforced...
 * Thus we must not use epicsShareAPI for C2F(ezca) -- but it seems
 * deprecated anyways (see shareLib.h).
 */
epicsShareFunc void C2F(MPVA)();

int
MPVA(wchar_t *funcName)
{
unsigned i;
	for ( i=0; i < sizeof(Tab)/sizeof(Tab[0]); i++ ) {
		if ( wcscmp( funcName, Tab[i].name ) == 0 ) {
			addCStackFunction( Tab[i].name, Tab[i].f, L"mpva" );
			return 1;
		}
	}
	return 0;
}
