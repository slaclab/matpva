/* PVAccess client implementation
 *
 * Implements the ezca-compatible C interface using EPICS7
 * PVAccess (pvac) C++ client API.
 *
 * This replaces the EZCA library for PVAccess-based operation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <memory>
#include <sstream>
#include <stdexcept>

#include <pv/pvAccess.h>
#include <pv/clientFactory.h>
#include <pva/client.h>
#include <pv/pvData.h>
#include <pv/ntscalar.h>
#include <pv/ntscalarArray.h>
#include <pv/ntenum.h>
#include <pv/pvTimeStamp.h>
#include <pv/pvAlarm.h>
#include <pv/pvDisplay.h>
#include <pv/pvControl.h>
#include <pv/event.h>

#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsMutex.h>

#define epicsExportSharedSymbols
#include <shareLib.h>
#include "pvaClient.h"

namespace pvd = epics::pvData;
namespace pva = epics::pvAccess;

/* ==================== Internal State ==================== */

static float      pvaTimeout    = 5.0;   /* seconds */
static int        pvaRetryCount = 3;
static int        pvaDebug      = 0;
static int        pvaInitialized = 0;
static EzcaPollCb pvaPollCb     = NULL;

static epicsMutex pvaMutex;

/* Group operation state - for PVA we track operations and
 * report results at "end group" */
static int        inGroup       = 0;
static std::vector<int> groupResults;

/* PVAccess provider */
static std::shared_ptr<pvac::ClientProvider> pvaProvider;

/* Error message buffer */
static char *lastErrorMsg = NULL;

/* Monitor tracking */
struct MonitorEntry {
    pvac::MonitorSync  monitor;
    bool               hasNewValue;
    char               type;
    MonitorEntry() : hasNewValue(false), type(ezcaDouble) {}
};

static std::map<std::string, std::shared_ptr<MonitorEntry>> monitorMap;

/* Channel cache */
static std::map<std::string, pvac::ClientChannel> channelCache;

/* ==================== Helper Functions ==================== */

static void debugPrint(const char *fmt, ...)
{
    if (!pvaDebug) return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

static void setLastError(const char *fmt, ...)
{
    va_list ap;
    char buf[1024];
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (lastErrorMsg) {
        free(lastErrorMsg);
    }
    lastErrorMsg = strdup(buf);
}

static pvac::ClientChannel& getChannel(const char *pvname)
{
    std::string name(pvname);
    auto it = channelCache.find(name);
    if (it == channelCache.end()) {
        if (!pvaProvider) {
            throw std::runtime_error("PVAccess provider not initialized");
        }
        channelCache.emplace(name, pvaProvider->connect(name));
        it = channelCache.find(name);
    }
    return it->second;
}

/* Convert PVA ScalarType to our ezca-compatible type code */
static char scalarTypeToEzca(pvd::ScalarType stype)
{
    switch (stype) {
        case pvd::pvByte:
        case pvd::pvUByte:   return ezcaByte;
        case pvd::pvShort:
        case pvd::pvUShort:  return ezcaShort;
        case pvd::pvInt:
        case pvd::pvUInt:
        case pvd::pvLong:
        case pvd::pvULong:   return ezcaLong;
        case pvd::pvFloat:   return ezcaFloat;
        case pvd::pvDouble:  return ezcaDouble;
        case pvd::pvString:  return ezcaString;
        default:             return ezcaFloat;
    }
}

/* Extract scalar value from a PVField into buffer based on target type */
static int extractScalarValue(pvd::PVFieldPtr field, char type, void *buf, int index)
{
    if (!field) return EZCA_CAFAILURE;

    try {
        pvd::PVScalarPtr scalar = std::dynamic_pointer_cast<pvd::PVScalar>(field);
        if (scalar) {
            switch (type) {
                case ezcaByte:
                    ((epicsInt8*)buf)[index] = scalar->getAs<epicsInt8>();
                    break;
                case ezcaShort:
                    ((epicsInt16*)buf)[index] = scalar->getAs<epicsInt16>();
                    break;
                case ezcaLong:
                    ((epicsInt32*)buf)[index] = scalar->getAs<epicsInt32>();
                    break;
                case ezcaFloat:
                    ((float*)buf)[index] = scalar->getAs<float>();
                    break;
                case ezcaDouble:
                    ((double*)buf)[index] = scalar->getAs<double>();
                    break;
                case ezcaString: {
                    std::string s = scalar->getAs<std::string>();
                    strncpy(((dbr_string_t*)buf)[index], s.c_str(), sizeof(dbr_string_t)-1);
                    ((dbr_string_t*)buf)[index][sizeof(dbr_string_t)-1] = '\0';
                    break;
                }
            }
            return EZCA_OK;
        }
    } catch (std::exception &e) {
        debugPrint("extractScalarValue: %s\n", e.what());
    }
    return EZCA_CAFAILURE;
}

/* Extract array values from PVScalarArray */
static int extractArrayValues(pvd::PVScalarArrayPtr arr, char type, void *buf, int nelem)
{
    if (!arr) return EZCA_CAFAILURE;

    try {
        size_t len = arr->getLength();
        if ((size_t)nelem > len) nelem = (int)len;

        switch (type) {
            case ezcaByte: {
                pvd::shared_vector<const epicsInt8> data;
                arr->getAs<epicsInt8>(data);
                for (int i = 0; i < nelem && i < (int)data.size(); i++)
                    ((epicsInt8*)buf)[i] = data[i];
                break;
            }
            case ezcaShort: {
                pvd::shared_vector<const epicsInt16> data;
                arr->getAs<epicsInt16>(data);
                for (int i = 0; i < nelem && i < (int)data.size(); i++)
                    ((epicsInt16*)buf)[i] = data[i];
                break;
            }
            case ezcaLong: {
                pvd::shared_vector<const epicsInt32> data;
                arr->getAs<epicsInt32>(data);
                for (int i = 0; i < nelem && i < (int)data.size(); i++)
                    ((epicsInt32*)buf)[i] = data[i];
                break;
            }
            case ezcaFloat: {
                pvd::shared_vector<const float> data;
                arr->getAs<float>(data);
                for (int i = 0; i < nelem && i < (int)data.size(); i++)
                    ((float*)buf)[i] = data[i];
                break;
            }
            case ezcaDouble: {
                pvd::shared_vector<const double> data;
                arr->getAs<double>(data);
                for (int i = 0; i < nelem && i < (int)data.size(); i++)
                    ((double*)buf)[i] = data[i];
                break;
            }
            case ezcaString: {
                pvd::shared_vector<const std::string> data;
                arr->getAs<std::string>(data);
                for (int i = 0; i < nelem && i < (int)data.size(); i++) {
                    strncpy(((dbr_string_t*)buf)[i], data[i].c_str(), sizeof(dbr_string_t)-1);
                    ((dbr_string_t*)buf)[i][sizeof(dbr_string_t)-1] = '\0';
                }
                break;
            }
        }
        return EZCA_OK;
    } catch (std::exception &e) {
        debugPrint("extractArrayValues: %s\n", e.what());
        setLastError("extractArrayValues: %s", e.what());
    }
    return EZCA_CAFAILURE;
}

/* Extract timestamp from PVStructure */
static void extractTimeStamp(pvd::PVStructurePtr pvs, epicsTimeStamp *ts)
{
    if (!ts) return;
    memset(ts, 0, sizeof(*ts));

    try {
        pvd::PVStructurePtr tsField = pvs->getSubField<pvd::PVStructure>("timeStamp");
        if (tsField) {
            pvd::PVLongPtr secField = tsField->getSubField<pvd::PVLong>("secondsPastEpoch");
            pvd::PVIntPtr nanoField = tsField->getSubField<pvd::PVInt>("nanoseconds");
            if (secField && nanoField) {
                /* PVA epoch is POSIX epoch; EPICS epoch is 1990-01-01 */
                static const int64_t EPICS_EPOCH_OFFSET = 631152000LL;
                ts->secPastEpoch = (epicsUInt32)(secField->get() - EPICS_EPOCH_OFFSET);
                ts->nsec = (epicsUInt32)nanoField->get();
            }
        }
    } catch (...) {}
}

/* Extract alarm status/severity from PVStructure */
static void extractAlarm(pvd::PVStructurePtr pvs, short *status, short *severity)
{
    if (status) *status = 0;
    if (severity) *severity = 0;

    try {
        pvd::PVStructurePtr alarmField = pvs->getSubField<pvd::PVStructure>("alarm");
        if (alarmField) {
            pvd::PVIntPtr sevField = alarmField->getSubField<pvd::PVInt>("severity");
            pvd::PVIntPtr statField = alarmField->getSubField<pvd::PVInt>("status");
            if (sevField && severity) *severity = (short)sevField->get();
            if (statField && status) *status = (short)statField->get();
        }
    } catch (...) {}
}

/* ==================== Public API ==================== */

int epicsShareAPI pvaClientInit(void)
{
    epicsMutex::guard_t G(pvaMutex);
    if (pvaInitialized) return EZCA_OK;

    try {
        pva::ClientFactory::start();
        pvaProvider = std::make_shared<pvac::ClientProvider>("pva");
        pvaInitialized = 1;
        debugPrint("PVAccess client initialized\n");
        return EZCA_OK;
    } catch (std::exception &e) {
        fprintf(stderr, "pvaClientInit failed: %s\n", e.what());
        return EZCA_CAFAILURE;
    }
}

void epicsShareAPI pvaClientShutdown(void)
{
    epicsMutex::guard_t G(pvaMutex);
    if (!pvaInitialized) return;

    monitorMap.clear();
    channelCache.clear();
    pvaProvider.reset();
    pva::ClientFactory::stop();
    pvaInitialized = 0;
    debugPrint("PVAccess client shut down\n");
}

int epicsShareAPI ezcaGetRetryCount(void) { return pvaRetryCount; }
float epicsShareAPI ezcaGetTimeout(void) { return pvaTimeout; }

int epicsShareAPI ezcaSetRetryCount(int retry)
{
    if (retry < 0) return EZCA_INVALIDARG;
    pvaRetryCount = retry;
    return EZCA_OK;
}

int epicsShareAPI ezcaSetTimeout(float sec)
{
    if (sec <= 0.0) return EZCA_INVALIDARG;
    pvaTimeout = sec;
    return EZCA_OK;
}

void epicsShareAPI ezcaDebugOn(void)  { pvaDebug = 1; }
void epicsShareAPI ezcaDebugOff(void) { pvaDebug = 0; }

int epicsShareAPI ezcaDelay(float sec)
{
    if (sec > 0.0) {
        epicsThreadSleep((double)sec);
    }
    return EZCA_OK;
}

void epicsShareAPI ezcaAbort(void)
{
    /* For PVA, cancellation is handled via timeouts */
    pvaRetryCount = 0;
}

void epicsShareAPI ezcaLock(void)   { pvaMutex.lock(); }
void epicsShareAPI ezcaUnlock(void) { pvaMutex.unlock(); }

EzcaPollCb epicsShareAPI ezcaPollCbInstall(EzcaPollCb cb)
{
    EzcaPollCb old = pvaPollCb;
    pvaPollCb = cb;
    return old;
}

void epicsShareAPI ezcaFree(void *buff)
{
    free(buff);
}

int epicsShareAPI ezcaGetErrorString(char *prefix, char **buff)
{
    if (!buff) return EZCA_INVALIDARG;

    std::string msg;
    if (prefix) msg = prefix;
    if (lastErrorMsg) {
        msg += lastErrorMsg;
    } else {
        msg += "OK\n";
    }
    *buff = strdup(msg.c_str());
    return EZCA_OK;
}

void epicsShareAPI ezcaPerror(char *prefix)
{
    char *buf = NULL;
    ezcaGetErrorString(prefix, &buf);
    if (buf) {
        fprintf(stderr, "%s", buf);
        free(buf);
    }
}

void epicsShareAPI ezcaAutoErrorMessageOff(void) { /* no-op for PVA */ }
void epicsShareAPI ezcaAutoErrorMessageOn(void) { /* no-op for PVA */ }

/* ---- Core PV Operations via PVAccess ---- */

int epicsShareAPI ezcaGet(char *pvname, char ezcatype, int nelem, void *data_buff)
{
    epicsTimeStamp ts;
    short st, sv;
    return ezcaGetWithStatus(pvname, ezcatype, nelem, data_buff, &ts, &st, &sv);
}

int epicsShareAPI ezcaGetWithStatus(char *pvname, char ezcatype, int nelem,
    void *data_buff, epicsTimeStamp *timestamp, short *status, short *severity)
{
    if (!pvname || !data_buff || nelem <= 0)
        return EZCA_INVALIDARG;

    debugPrint("ezcaGetWithStatus: %s type=%d nelem=%d\n", pvname, ezcatype, nelem);

    try {
        pvac::ClientChannel &ch = getChannel(pvname);
        pvd::PVStructure::const_shared_pointer result = ch.get(pvaTimeout);

        if (!result) {
            setLastError("Get failed for PV '%s': no data returned\n", pvname);
            return EZCA_NOTIMELYRESPONSE;
        }

        /* Extract timestamp and alarm */
        pvd::PVStructurePtr mutableResult = std::const_pointer_cast<pvd::PVStructure>(result);
        extractTimeStamp(mutableResult, timestamp);
        extractAlarm(mutableResult, status, severity);

        /* Get the value field */
        pvd::PVFieldPtr valueField = mutableResult->getSubField("value");
        if (!valueField) {
            setLastError("PV '%s' has no 'value' field\n", pvname);
            return EZCA_CAFAILURE;
        }

        /* Check if it's a scalar or array */
        pvd::PVScalarPtr scalar = std::dynamic_pointer_cast<pvd::PVScalar>(valueField);
        if (scalar) {
            return extractScalarValue(valueField, ezcatype, data_buff, 0);
        }

        pvd::PVScalarArrayPtr array = std::dynamic_pointer_cast<pvd::PVScalarArray>(valueField);
        if (array) {
            return extractArrayValues(array, ezcatype, data_buff, nelem);
        }

        /* Could be an enum - check for index subfield */
        pvd::PVStructurePtr valStruct = std::dynamic_pointer_cast<pvd::PVStructure>(valueField);
        if (valStruct) {
            pvd::PVScalarPtr indexField = valStruct->getSubField<pvd::PVScalar>("index");
            if (indexField) {
                return extractScalarValue(indexField, ezcatype, data_buff, 0);
            }
        }

        setLastError("PV '%s': unsupported value type\n", pvname);
        return EZCA_CAFAILURE;

    } catch (pvac::Timeout &) {
        setLastError("Timeout getting PV '%s'\n", pvname);
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::runtime_error &e) {
        setLastError("Get failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_NOTCONNECTED;
    } catch (std::exception &e) {
        setLastError("Get error for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

static int doPut(char *pvname, char ezcatype, int nelem, void *data_buff, bool wait)
{
    if (!pvname || !data_buff || nelem <= 0)
        return EZCA_INVALIDARG;

    debugPrint("ezcaPut: %s type=%d nelem=%d wait=%d\n", pvname, ezcatype, nelem, wait);

    try {
        pvac::ClientChannel &ch = getChannel(pvname);

        /* Build the put value */
        if (nelem == 1) {
            /* Scalar put */
            pvac::ClientChannel::PutBuilder builder = ch.put();
            switch (ezcatype) {
                case ezcaByte:
                    builder.set("value", (int)((epicsInt8*)data_buff)[0]);
                    break;
                case ezcaShort:
                    builder.set("value", (int)((epicsInt16*)data_buff)[0]);
                    break;
                case ezcaLong:
                    builder.set("value", (int)((epicsInt32*)data_buff)[0]);
                    break;
                case ezcaFloat:
                    builder.set("value", (double)((float*)data_buff)[0]);
                    break;
                case ezcaDouble:
                    builder.set("value", ((double*)data_buff)[0]);
                    break;
                case ezcaString: {
                    std::string s(((dbr_string_t*)data_buff)[0]);
                    builder.set("value", s);
                    break;
                }
            }
            builder.exec(pvaTimeout);
        } else {
            /* Array put */
            pvac::ClientChannel::PutBuilder builder = ch.put();
            switch (ezcatype) {
                case ezcaByte: {
                    pvd::shared_vector<epicsInt8> vec(nelem);
                    memcpy(vec.data(), data_buff, nelem * sizeof(epicsInt8));
                    builder.set("value", pvd::freeze(vec));
                    break;
                }
                case ezcaShort: {
                    pvd::shared_vector<epicsInt16> vec(nelem);
                    memcpy(vec.data(), data_buff, nelem * sizeof(epicsInt16));
                    builder.set("value", pvd::freeze(vec));
                    break;
                }
                case ezcaLong: {
                    pvd::shared_vector<epicsInt32> vec(nelem);
                    memcpy(vec.data(), data_buff, nelem * sizeof(epicsInt32));
                    builder.set("value", pvd::freeze(vec));
                    break;
                }
                case ezcaFloat: {
                    pvd::shared_vector<float> vec(nelem);
                    memcpy(vec.data(), data_buff, nelem * sizeof(float));
                    builder.set("value", pvd::freeze(vec));
                    break;
                }
                case ezcaDouble: {
                    pvd::shared_vector<double> vec(nelem);
                    memcpy(vec.data(), data_buff, nelem * sizeof(double));
                    builder.set("value", pvd::freeze(vec));
                    break;
                }
                case ezcaString: {
                    pvd::shared_vector<std::string> vec(nelem);
                    for (int i = 0; i < nelem; i++)
                        vec[i] = std::string(((dbr_string_t*)data_buff)[i]);
                    builder.set("value", pvd::freeze(vec));
                    break;
                }
            }
            builder.exec(pvaTimeout);
        }
        return EZCA_OK;

    } catch (pvac::Timeout &) {
        setLastError("Timeout putting PV '%s'\n", pvname);
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::runtime_error &e) {
        setLastError("Put failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_NOTCONNECTED;
    } catch (std::exception &e) {
        setLastError("Put error for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

int epicsShareAPI ezcaPut(char *pvname, char ezcatype, int nelem, void *data_buff)
{
    return doPut(pvname, ezcatype, nelem, data_buff, true);
}

int epicsShareAPI ezcaPutOldCa(char *pvname, char ezcatype, int nelem, void *data_buff)
{
    /* For PVAccess, fire-and-forget is not directly supported the same way.
     * We do a regular put but with shorter timeout (best effort). */
    return doPut(pvname, ezcatype, nelem, data_buff, false);
}

int epicsShareAPI ezcaGetNelem(char *pvname, int *nelem)
{
    if (!pvname || !nelem) return EZCA_INVALIDARG;

    try {
        pvac::ClientChannel &ch = getChannel(pvname);
        pvd::PVStructure::const_shared_pointer result = ch.get(pvaTimeout);

        if (!result) {
            *nelem = 0;
            return EZCA_NOTIMELYRESPONSE;
        }

        pvd::PVStructurePtr mutableResult = std::const_pointer_cast<pvd::PVStructure>(result);
        pvd::PVFieldPtr valueField = mutableResult->getSubField("value");
        if (!valueField) {
            *nelem = 0;
            return EZCA_CAFAILURE;
        }

        pvd::PVScalarPtr scalar = std::dynamic_pointer_cast<pvd::PVScalar>(valueField);
        if (scalar) {
            *nelem = 1;
            return EZCA_OK;
        }

        pvd::PVScalarArrayPtr array = std::dynamic_pointer_cast<pvd::PVScalarArray>(valueField);
        if (array) {
            *nelem = (int)array->getLength();
            return EZCA_OK;
        }

        /* Enum type - scalar */
        *nelem = 1;
        return EZCA_OK;

    } catch (pvac::Timeout &) {
        setLastError("Timeout getting nelem for PV '%s'\n", pvname);
        *nelem = 0;
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::exception &e) {
        setLastError("GetNelem error for PV '%s': %s\n", pvname, e.what());
        *nelem = 0;
        return EZCA_NOTCONNECTED;
    }
}

/* ---- Metadata Operations ---- */

static int getDoublePair(char *pvname, const char *structName,
    const char *lowField, const char *highField, double *low, double *high)
{
    if (!pvname || !low || !high) return EZCA_INVALIDARG;

    try {
        pvac::ClientChannel &ch = getChannel(pvname);
        pvd::PVStructure::const_shared_pointer result = ch.get(pvaTimeout,
            std::string("field(") + structName + ")");

        if (!result) return EZCA_NOTIMELYRESPONSE;

        pvd::PVStructurePtr mutableResult = std::const_pointer_cast<pvd::PVStructure>(result);
        pvd::PVStructurePtr sub = mutableResult->getSubField<pvd::PVStructure>(structName);
        if (!sub) {
            *low = *high = 0.0;
            return EZCA_OK;
        }

        pvd::PVScalarPtr lowVal = sub->getSubField<pvd::PVScalar>(lowField);
        pvd::PVScalarPtr highVal = sub->getSubField<pvd::PVScalar>(highField);
        *low = lowVal ? lowVal->getAs<double>() : 0.0;
        *high = highVal ? highVal->getAs<double>() : 0.0;
        return EZCA_OK;

    } catch (pvac::Timeout &) {
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::exception &e) {
        setLastError("Metadata query failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

int epicsShareAPI ezcaGetControlLimits(char *pvname, double *low, double *high)
{
    return getDoublePair(pvname, "control", "limitLow", "limitHigh", low, high);
}

int epicsShareAPI ezcaGetGraphicLimits(char *pvname, double *low, double *high)
{
    return getDoublePair(pvname, "display", "limitLow", "limitHigh", low, high);
}

int epicsShareAPI ezcaGetWarnLimits(char *pvname, double *low, double *high)
{
    return getDoublePair(pvname, "valueAlarm", "lowWarningLimit", "highWarningLimit", low, high);
}

int epicsShareAPI ezcaGetAlarmLimits(char *pvname, double *low, double *high)
{
    return getDoublePair(pvname, "valueAlarm", "lowAlarmLimit", "highAlarmLimit", low, high);
}

int epicsShareAPI ezcaGetPrecision(char *pvname, short *precision)
{
    if (!pvname || !precision) return EZCA_INVALIDARG;

    try {
        pvac::ClientChannel &ch = getChannel(pvname);
        pvd::PVStructure::const_shared_pointer result = ch.get(pvaTimeout,
            "field(display)");

        if (!result) return EZCA_NOTIMELYRESPONSE;

        pvd::PVStructurePtr mutableResult = std::const_pointer_cast<pvd::PVStructure>(result);
        pvd::PVStructurePtr display = mutableResult->getSubField<pvd::PVStructure>("display");
        if (display) {
            pvd::PVStructurePtr format = display->getSubField<pvd::PVStructure>("format");
            if (format) {
                pvd::PVScalarPtr precField = format->getSubField<pvd::PVScalar>("precision");
                if (precField) {
                    *precision = precField->getAs<int16_t>();
                    return EZCA_OK;
                }
            }
            /* Try legacy 'precision' field */
            pvd::PVScalarPtr precField = display->getSubField<pvd::PVScalar>("precision");
            if (precField) {
                *precision = precField->getAs<int16_t>();
                return EZCA_OK;
            }
        }
        *precision = 0;
        return EZCA_OK;

    } catch (pvac::Timeout &) {
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::exception &e) {
        setLastError("GetPrecision failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

int epicsShareAPI ezcaGetStatus(char *pvname, epicsTimeStamp *timestamp,
    short *status, short *severity)
{
    if (!pvname) return EZCA_INVALIDARG;

    try {
        pvac::ClientChannel &ch = getChannel(pvname);
        pvd::PVStructure::const_shared_pointer result = ch.get(pvaTimeout,
            "field(alarm,timeStamp)");

        if (!result) return EZCA_NOTIMELYRESPONSE;

        pvd::PVStructurePtr mutableResult = std::const_pointer_cast<pvd::PVStructure>(result);
        extractTimeStamp(mutableResult, timestamp);
        extractAlarm(mutableResult, status, severity);
        return EZCA_OK;

    } catch (pvac::Timeout &) {
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::exception &e) {
        setLastError("GetStatus failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

int epicsShareAPI ezcaGetUnits(char *pvname, char *units)
{
    if (!pvname || !units) return EZCA_INVALIDARG;

    try {
        pvac::ClientChannel &ch = getChannel(pvname);
        pvd::PVStructure::const_shared_pointer result = ch.get(pvaTimeout,
            "field(display)");

        if (!result) return EZCA_NOTIMELYRESPONSE;

        pvd::PVStructurePtr mutableResult = std::const_pointer_cast<pvd::PVStructure>(result);
        pvd::PVStructurePtr display = mutableResult->getSubField<pvd::PVStructure>("display");
        if (display) {
            pvd::PVStringPtr unitsField = display->getSubField<pvd::PVString>("units");
            if (unitsField) {
                strncpy(units, unitsField->get().c_str(), EZCA_UNITS_SIZE - 1);
                units[EZCA_UNITS_SIZE - 1] = '\0';
                return EZCA_OK;
            }
        }
        units[0] = '\0';
        return EZCA_OK;

    } catch (pvac::Timeout &) {
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::exception &e) {
        setLastError("GetUnits failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

int epicsShareAPI ezcaGetEnumStrings(char *pvname,
    char states[EZCA_ENUM_STATES][EZCA_ENUM_STRING_SIZE])
{
    if (!pvname || !states) return EZCA_INVALIDARG;

    memset(states, 0, EZCA_ENUM_STATES * EZCA_ENUM_STRING_SIZE);

    try {
        pvac::ClientChannel &ch = getChannel(pvname);
        pvd::PVStructure::const_shared_pointer result = ch.get(pvaTimeout,
            "field(value)");

        if (!result) return EZCA_NOTIMELYRESPONSE;

        pvd::PVStructurePtr mutableResult = std::const_pointer_cast<pvd::PVStructure>(result);

        /* Look for enum choices in value.choices */
        pvd::PVStructurePtr valueStruct = mutableResult->getSubField<pvd::PVStructure>("value");
        if (valueStruct) {
            pvd::PVStringArrayPtr choices = valueStruct->getSubField<pvd::PVStringArray>("choices");
            if (choices) {
                pvd::shared_vector<const std::string> data;
                choices->getAs(data);
                for (size_t i = 0; i < data.size() && i < EZCA_ENUM_STATES; i++) {
                    strncpy(states[i], data[i].c_str(), EZCA_ENUM_STRING_SIZE - 1);
                    states[i][EZCA_ENUM_STRING_SIZE - 1] = '\0';
                }
                return EZCA_OK;
            }
        }
        return EZCA_OK;

    } catch (pvac::Timeout &) {
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::exception &e) {
        setLastError("GetEnumStrings failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

/* ---- Monitor Operations ---- */

int epicsShareAPI ezcaSetMonitor(char *pvname, char ezcatype, unsigned long count)
{
    if (!pvname) return EZCA_INVALIDARG;

    std::string name(pvname);
    epicsMutex::guard_t G(pvaMutex);

    try {
        /* Remove existing monitor if any */
        monitorMap.erase(name);

        pvac::ClientChannel &ch = getChannel(pvname);
        auto entry = std::make_shared<MonitorEntry>();
        entry->type = ezcatype;
        entry->monitor = ch.monitor();
        entry->hasNewValue = false;
        monitorMap[name] = entry;
        return EZCA_OK;

    } catch (std::exception &e) {
        setLastError("SetMonitor failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

int epicsShareAPI ezcaClearMonitor(char *pvname, char ezcatype)
{
    if (!pvname) return EZCA_INVALIDARG;
    (void)ezcatype;

    std::string name(pvname);
    epicsMutex::guard_t G(pvaMutex);
    monitorMap.erase(name);
    return EZCA_OK;
}

int epicsShareAPI ezcaNewMonitorValue(char *pvname, char ezcatype)
{
    if (!pvname) return -1;
    (void)ezcatype;

    std::string name(pvname);
    epicsMutex::guard_t G(pvaMutex);

    auto it = monitorMap.find(name);
    if (it == monitorMap.end()) return -1; /* no monitor set */

    try {
        /* Check if there's a new event in the monitor queue */
        if (it->second->monitor.poll()) {
            it->second->hasNewValue = true;
            return 1; /* TRUE - new value available */
        }
        return 0; /* FALSE - no new value */
    } catch (...) {
        return -2; /* error */
    }
}

int epicsShareAPI ezcaNewMonitorWait(char *pvname, char ezcatype)
{
    if (!pvname) return EZCA_INVALIDARG;
    (void)ezcatype;

    std::string name(pvname);

    std::shared_ptr<MonitorEntry> entry;
    {
        epicsMutex::guard_t G(pvaMutex);
        auto it = monitorMap.find(name);
        if (it == monitorMap.end()) return EZCA_INVALIDARG;
        entry = it->second;
    }

    try {
        /* Wait for a monitor event with timeout */
        if (entry->monitor.wait(pvaTimeout)) {
            if (entry->monitor.poll()) {
                entry->hasNewValue = true;
                return EZCA_OK;
            }
        }
        return EZCA_NOTIMELYRESPONSE;
    } catch (std::exception &e) {
        setLastError("MonitorWait failed for PV '%s': %s\n", pvname, e.what());
        return EZCA_CAFAILURE;
    }
}

/* ---- Channel Management ---- */

int epicsShareAPI ezcaClearChannel(char *pvname)
{
    if (!pvname) return EZCA_INVALIDARG;

    std::string name(pvname);
    epicsMutex::guard_t G(pvaMutex);

    monitorMap.erase(name);
    channelCache.erase(name);
    return EZCA_OK;
}

int epicsShareAPI ezcaPurge(int disconnectedOnly)
{
    epicsMutex::guard_t G(pvaMutex);
    if (!disconnectedOnly) {
        monitorMap.clear();
        channelCache.clear();
    }
    /* For PVAccess, disconnected channels are handled by the provider.
     * We just clear the cache. */
    return EZCA_OK;
}

/* ---- Group Operations (PVA compatibility) ---- */
/* In PVAccess, operations are issued immediately rather than batched.
 * The group API is maintained for interface compatibility. */

int epicsShareAPI ezcaStartGroup(void)
{
    inGroup = 1;
    groupResults.clear();
    return EZCA_OK;
}

int epicsShareAPI ezcaEndGroup(void)
{
    inGroup = 0;
    for (int rc : groupResults) {
        if (rc != EZCA_OK) return rc;
    }
    groupResults.clear();
    return EZCA_OK;
}

int epicsShareAPI ezcaEndGroupWithReport(int **rcs, int *nrcs)
{
    inGroup = 0;
    int overallRc = EZCA_OK;

    if (groupResults.empty()) {
        *rcs = NULL;
        *nrcs = 0;
        return EZCA_OK;
    }

    *nrcs = (int)groupResults.size();
    *rcs = (int*)malloc((*nrcs) * sizeof(int));
    if (!*rcs) {
        *nrcs = 0;
        return EZCA_FAILEDMALLOC;
    }

    for (int i = 0; i < *nrcs; i++) {
        (*rcs)[i] = groupResults[i];
        if (groupResults[i] != EZCA_OK)
            overallRc = groupResults[i];
    }
    groupResults.clear();
    return overallRc;
}

int epicsShareAPI ezcaGetFieldType(char *pvname)
{
    if (!pvname) return TYPENOTCONN;

    try {
        pvac::ClientChannel &ch = getChannel(pvname);
        pvd::PVStructure::const_shared_pointer result = ch.get(pvaTimeout, "field(value)");

        if (!result) return TYPENOTCONN;

        pvd::PVStructurePtr mutableResult = std::const_pointer_cast<pvd::PVStructure>(result);
        pvd::PVFieldPtr valueField = mutableResult->getSubField("value");
        if (!valueField) return TYPENOTCONN;

        pvd::PVScalarPtr scalar = std::dynamic_pointer_cast<pvd::PVScalar>(valueField);
        if (scalar) {
            switch (scalar->getScalar()->getScalarType()) {
                case pvd::pvByte:
                case pvd::pvUByte:   return DBF_CHAR;
                case pvd::pvShort:
                case pvd::pvUShort:  return DBF_SHORT;
                case pvd::pvInt:
                case pvd::pvUInt:
                case pvd::pvLong:
                case pvd::pvULong:   return DBF_LONG;
                case pvd::pvFloat:   return DBF_FLOAT;
                case pvd::pvDouble:  return DBF_DOUBLE;
                case pvd::pvString:  return DBF_STRING;
                default:             return DBF_FLOAT;
            }
        }

        pvd::PVScalarArrayPtr array = std::dynamic_pointer_cast<pvd::PVScalarArray>(valueField);
        if (array) {
            switch (array->getScalarArray()->getElementType()) {
                case pvd::pvByte:
                case pvd::pvUByte:   return DBF_CHAR;
                case pvd::pvShort:
                case pvd::pvUShort:  return DBF_SHORT;
                case pvd::pvInt:
                case pvd::pvUInt:
                case pvd::pvLong:
                case pvd::pvULong:   return DBF_LONG;
                case pvd::pvFloat:   return DBF_FLOAT;
                case pvd::pvDouble:  return DBF_DOUBLE;
                case pvd::pvString:  return DBF_STRING;
                default:             return DBF_FLOAT;
            }
        }

        /* Check for enum struct */
        pvd::PVStructurePtr valStruct = std::dynamic_pointer_cast<pvd::PVStructure>(valueField);
        if (valStruct && valStruct->getSubField("index")) {
            return DBF_ENUM;
        }

        return DBF_FLOAT;

    } catch (...) {
        return TYPENOTCONN;
    }
}
