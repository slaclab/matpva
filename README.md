# MPVA — EPICS PVAccess Interface for SCILAB and MATLAB

## Overview

MPVA provides a PVAccess (PVA) protocol interface for SCILAB and MATLAB,
enabling access to EPICS Process Variables using the modern PVAccess
protocol (part of EPICS 7). PVAccess supports structured data types,
atomic multi-field access, and is the recommended protocol for new
EPICS applications.

This project is derived from the labCA Channel Access interface and has been
converted to use PVAccess as the underlying transport protocol. The user-facing
API (function names and calling conventions) remains similar for ease of
migration.

## Available Functions

| Function | Description |
|----------|-------------|
| `mpvaGet` | Get PV value(s) |
| `mpvaPut` | Put value(s) to PV(s) |
| `mpvaPutNoWait` | Put without waiting for completion |
| `mpvaGetNelem` | Get native element count of PV(s) |
| `mpvaGetStatus` | Get PV alarm status and severity |
| `mpvaGetPrecision` | Get PV display precision |
| `mpvaGetUnits` | Get PV engineering units |
| `mpvaGetEnumStrings` | Get PV enum string labels |
| `mpvaGetWarnLimits` | Get PV warning limits |
| `mpvaGetAlarmLimits` | Get PV alarm limits |
| `mpvaGetControlLimits` | Get PV control limits |
| `mpvaGetGraphicLimits` | Get PV graphic/display limits |
| `mpvaGetRetryCount` | Get connection retry count |
| `mpvaSetRetryCount` | Set connection retry count |
| `mpvaGetTimeout` | Get connection timeout |
| `mpvaSetTimeout` | Set connection timeout |
| `mpvaSetSeverityWarnLevel` | Set severity warning threshold |
| `mpvaSetMonitor` | Set up a PV monitor (subscription) |
| `mpvaNewMonitorValue` | Check if new monitor data is available |
| `mpvaNewMonitorWait` | Wait for new monitor data |
| `mpvaClear` | Disconnect/clear PV channel(s) |
| `mpvaLastError` | Get last error information |
| `mpvaDelay` | Delay and process background activities |
| `mpvaDebugOn` | Enable debug output |
| `mpvaDebugOff` | Disable debug output |

## Quick Start

### Run Instructions

 - Point your `PATH` to `bin/<arch>`
 - Point your `LD_LIBRARY_PATH` to `lib/<arch>` (Linux only)
 - Your `PATH` (Win32) or `LD_LIBRARY_PATH` (Linux) may also need to
   include your EPICS base `bin/<arch>` or `lib/<arch>` directory

#### MATLAB

```matlab
addpath bin/<arch>/mpva
help mpva
```

> **Note:** If MATLAB 2020b hangs, try setting `LD_PRELOAD` before launching:
> ```
> setenv LD_PRELOAD <path_to_epics_base>/lib/<arch>/libCom.so
> ```

#### SCILAB

```scilab
exec bin/<arch>/mpva.sce
help mpva
```

## Build From Source

### Prerequisites

- **EPICS 7** (or later) — required for PVAccess (`pvAccess`, `pvData` libraries
  are included in EPICS 7 base)
- **MATLAB** or **SCILAB**
- GNU Make

### Steps

1. Edit `configure/CONFIG` and set target application
   (`MAKEFOR=MATLAB` or `MAKEFOR=SCILAB`) and optionally the
   installation directory. You can also enable experimental Ctrl-C
   handling to interrupt `mpvaGet`/`mpvaPut` operations.

2. Edit `configure/RELEASE` to point to your installation directories of:
   - EPICS 7 base (must include pvAccess and pvData)
   - MATLAB
   - SCILAB

   On Win32, also set `MATLIB_SUBDIR` to point to your compiler-dependent
   libmx/libmex versions.

3. Set the `EPICS_HOST_ARCH` environment variable.

4. Run GNU make from this directory.

> Building documentation requires pdflatex, latex2html, and lynx.
> Documentation build is skipped if any of these tools is missing.

## License

See [LICENSE](LICENSE) for details.
