/* PVAccess client compatibility layer
 *
 * Provides an interface similar to EZCA but implemented using
 * EPICS7 PVAccess (pvac) C++ client API.
 *
 * This replaces the ezca library for PVAccess-based operation.
 */

#ifndef PVA_CLIENT_H
#define PVA_CLIENT_H

#include <epicsTime.h>
#include <shareLib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Data Types - compatible with ezca type codes */
#define ezcaByte   0
#define ezcaString 1
#define ezcaShort  2
#define ezcaLong   3
#define ezcaFloat  4
#define ezcaDouble 5
#define VALID_EZCA_DATA_TYPE(X) (((X) >= 0)&&((X)<=(ezcaDouble)))

/* Return Codes - compatible with ezca error codes */
#define EZCA_OK                0
#define EZCA_INVALIDARG        1
#define EZCA_FAILEDMALLOC      2
#define EZCA_CAFAILURE         3
#define EZCA_UDFREQ            4
#define EZCA_NOTCONNECTED      5
#define EZCA_NOTIMELYRESPONSE  6
#define EZCA_INGROUP           7
#define EZCA_NOTINGROUP        8
#define EZCA_ABORTED           9
#define EZCA_INTERNALERR      10

/* must match size of enum string in NTEnum */
#define EZCA_UNITS_SIZE 64
#define EZCA_ENUM_STATES 16
#define EZCA_ENUM_STRING_SIZE 26

/* String type - buffer for PV string values */
typedef char dbr_string_t[40];

/* Field type constants (for nativeType queries) */
#define DBF_CHAR    0
#define DBF_SHORT   1
#define DBF_LONG    5
#define DBF_FLOAT   6
#define DBF_DOUBLE   7
#define DBF_STRING  0
#define DBF_ENUM    3
#define TYPENOTCONN -1

/* PVAccess provider initialization/shutdown */
epicsShareFunc int epicsShareAPI pvaClientInit(void);
epicsShareFunc void epicsShareAPI pvaClientShutdown(void);

/* Timeout and retry control */
epicsShareFunc int epicsShareAPI ezcaGetRetryCount(void);
epicsShareFunc float epicsShareAPI ezcaGetTimeout(void);
epicsShareFunc int epicsShareAPI ezcaSetRetryCount(int retry);
epicsShareFunc int epicsShareAPI ezcaSetTimeout(float sec);

/* Debug control */
epicsShareFunc void epicsShareAPI ezcaDebugOn(void);
epicsShareFunc void epicsShareAPI ezcaDebugOff(void);

/* Delay / process background activities */
epicsShareFunc int epicsShareAPI ezcaDelay(float sec);

/* Abort pending operations */
epicsShareFunc void epicsShareAPI ezcaAbort(void);

/* Lock/unlock (thread safety) */
epicsShareFunc void epicsShareAPI ezcaLock(void);
epicsShareFunc void epicsShareAPI ezcaUnlock(void);

/* Poll callback type */
typedef int (*EzcaPollCb)();
epicsShareFunc EzcaPollCb epicsShareAPI ezcaPollCbInstall(EzcaPollCb);

/* Memory management */
epicsShareFunc void epicsShareAPI ezcaFree(void *buff);

/* Error reporting */
epicsShareFunc int epicsShareAPI ezcaGetErrorString(char *prefix, char **buff);
epicsShareFunc void epicsShareAPI ezcaPerror(char *prefix);
epicsShareFunc void epicsShareAPI ezcaAutoErrorMessageOff(void);
epicsShareFunc void epicsShareAPI ezcaAutoErrorMessageOn(void);

/* Channel operations via PVAccess */
epicsShareFunc int epicsShareAPI ezcaGet(char *pvname, char ezcatype,
    int nelem, void *data_buff);
epicsShareFunc int epicsShareAPI ezcaGetWithStatus(char *pvname,
    char ezcatype, int nelem, void *data_buff, epicsTimeStamp *timestamp,
    short *status, short *severity);
epicsShareFunc int epicsShareAPI ezcaPut(char *pvname, char ezcatype,
    int nelem, void *data_buff);
epicsShareFunc int epicsShareAPI ezcaPutOldCa(char *pvname, char ezcatype,
    int nelem, void *data_buff);
epicsShareFunc int epicsShareAPI ezcaGetNelem(char *pvname, int *nelem);

/* Metadata queries */
epicsShareFunc int epicsShareAPI ezcaGetControlLimits(char *pvname,
    double *low, double *high);
epicsShareFunc int epicsShareAPI ezcaGetGraphicLimits(char *pvname,
    double *low, double *high);
epicsShareFunc int epicsShareAPI ezcaGetWarnLimits(char *pvname,
    double *low, double *high);
epicsShareFunc int epicsShareAPI ezcaGetAlarmLimits(char *pvname,
    double *low, double *high);
epicsShareFunc int epicsShareAPI ezcaGetPrecision(char *pvname,
    short *precision);
epicsShareFunc int epicsShareAPI ezcaGetStatus(char *pvname,
    epicsTimeStamp *timestamp, short *status, short *severity);
epicsShareFunc int epicsShareAPI ezcaGetUnits(char *pvname,
    char *units);
epicsShareFunc int epicsShareAPI ezcaGetEnumStrings(char *pvname,
    char states[EZCA_ENUM_STATES][EZCA_ENUM_STRING_SIZE]);

/* Monitor operations */
epicsShareFunc int epicsShareAPI ezcaSetMonitor(char *pvname, char ezcatype,
    unsigned long count);
epicsShareFunc int epicsShareAPI ezcaClearMonitor(char *pvname, char ezcatype);
epicsShareFunc int epicsShareAPI ezcaNewMonitorValue(char *pvname,
    char ezcatype);
epicsShareFunc int epicsShareAPI ezcaNewMonitorWait(char *pvname, char ezcatype);

/* Channel management */
epicsShareFunc int epicsShareAPI ezcaClearChannel(char *pvname);
epicsShareFunc int epicsShareAPI ezcaPurge(int disconnectedOnly);

/* Grouped operations - implemented as immediate execution for PVA */
epicsShareFunc int epicsShareAPI ezcaStartGroup(void);
epicsShareFunc int epicsShareAPI ezcaEndGroup(void);
epicsShareFunc int epicsShareAPI ezcaEndGroupWithReport(int **rcs, int *nrcs);

/* Get native field type for a PV */
epicsShareFunc int epicsShareAPI ezcaGetFieldType(char *pvname);

#ifdef __cplusplus
}
#endif

#endif /* PVA_CLIENT_H */
