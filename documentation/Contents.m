% EPICS PVAccess Interface
% ==============================
%
%   mpvaGet                   - read one or multiple EPICS PVs
%   mpvaPut                   - write one or multiple EPICS PVs
%   mpvaPutNoWait             - write one or multiple EPICS PVs without waiting
%                              for record processing to complete on the server
%   mpvaGetNelem              - retrieve max. number of elements of EPICS PVs
%   mpvaGetStatus             - read status, severity and timestamp of EPICS PVs
%   mpvaGetControlLimits      - read control limits of EPICS PVs
%   mpvaGetGraphicLimits      - read graphic limits of EPICS PVs
%   mpvaGetPrecision          - read the display precision associated with EPICS PVs
%   mpvaGetUnits              - read the engineering units string associated with EPICS PVs
%   mpvaGetEnumStrings         - read the ENUM strings defined for a PV
%   mpvaGetRetryCount         - read/set the EZCA library retry count parameter
%   mpvaGetTimeout            - read/set the EZCA library timeout parameter
%   mpvaDebugOn               - toggle EZCA library debugging messages on/off
%   mpvaSetSeverityWarnLevel  - set the warning threshold used when reading EPICS 'VAL' PVs
%   mpvaClear                 - clear (destroy/cleanup) channels and associated monitors
%   mpvaSetMonitor            - monitor a channel
%   mpvaNewMonitorValue       - check if a monitored channel has new data
%   mpvaNewMonitorWait        - block/wait until a monitored channel has new data
%   mpvaLastError             - return error code(s) reported by last faulting mpvaXXX operation
