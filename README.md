# Matpva: Integrate EPICS7 into MATLAB using PVAccess for Python (P4P) module

## Motivation
Many scientists in SLAC use MATLAB for data analysis. Thus, they need to use EPICS7 in the MATLAB environment. There were some efforts to integrate EPICS into MATLAB such as labca and MATLAB CA. However, these were specifically designed for the EPICS Channel Access (CA) interface within MATLAB. Consequently, they did not provide comprehensive support for EPICS PVAcess, which offers distinct advantages over CA including handling larger datasets such as NTTable. Therefore, Matpva is developed to integrate EPICS7 into MATLAB based on Python using the P4P module to use PVAcess.
<br /><br />

## Prerequisites
- Python: 3.8.13+
- p4p: 3.5.5+
- Matlab: 2020a+

> **Note**: While Matpva has been primarily tested in Python 3.8.13, p4p 3.5.5, Matlab 2020a, there is a possibility that certain earlier versions could also function correctly. If you want to generate PVs using either matlab_model_pvs.py or pva_testing_ioc.py scripts, which enable timeStamp updates, it is necessary to utilize p4p version 4.x+.

<br />

## Function and test scripts
Python scripts to run Test PVs: pva_testing_ioc.py, matlab_model_pvs.py

MATLAB scripts to test mpvaGet() and mpvaPut(): mpvaGet_test_script.m, mpvaPut_test_script.m 
<br /><br />

## Running test scripts
```
python pva_testing_ioc.py    # Most of testing PVs including NTScalar, NTScalarArray, NTTable data type
python matlab_model_pvs.py   # TWISS NTTable PV
```
<br />

## How to use
1. **mpvaGet**: mpvaGet returns the values of given EPICS PV names.
```
[PV, ts, alarm] = mpvaGet(pvname)     # When PV is NTScalar or NTScalarArray type
[NTTable, ts, alarm, NTStruct] = mpvaGet(pvname)      # When PV is NTTable type     
```
<br />

2. **mpvaPut**: mpvaPut puts the input values into the designated field in the given EPICs PV name.
```
mpvaPut(pvname, value)      # When PV is NTScalar or NTScalarArray type
mpvaPut(pvname, field1, value1, field2, value2, ...)      # When PV is NTTable type
mpvaPut(pvname, struct/table)     # When PV is NTTable type
```

> **Note**: By default, mpvaPut doesn't display anything. If you would like to print out previous and updated PVs, add "mpvaDebugOn" as the last argument input of the function as shown below:
```
mpvaPut(pvname, value, "mpvaDebugOn")   # When PV is NTScalar or NTScalarArray type
mpvaPut(pvname, field1, value1, field2, value2, ..., "mpvaDebugOn")     # When PV is NTTable type
mpvaPut(pvname, struct/table,  "mpvaDebugOn")       # When PV is NTTable type
```
<br />

3. **mpvaMonitor**: mpvaMonitor displays the values of given EPICS PV names when it is updated.
```
mpvaMonitor(pvname)
```
<br />

4. **mpvaSetMonitor**: mpvaSetMonitor subscribes to the given EPICS PV using Python class and stores values in the cache when it is updated.
The module includes a method (mpvaNewMonitorValue) to check if the PV is updated since it has been subscribed.
```
PV = mpvaSetMonitor(pvname)      # Instantiate a ValueCache classs in mpvaSetMonitor.py Python module to monitor a pvname
```
<br />

5. **mpvaNewMonitorValue**: mpvaNewMonitorValue returns false if the pvname is not updated and returns true if it is updated. This function is especially valuable when the read operation consumes a significant amount of time, such as when dealing with large arrays or NTTables.
```
mpvaNewMonitorValue(PV)          # Return false if the pvname is not updated. Return true if it is updated.
```
> **Note**: mpvaNewMonitorValue function only works when mpvaSetMonitor is instantiated.

<br />

## To Do
1. Find a good example of NTNDArray PV and Support NTNDArray type

<br />

## Documnetation
https://confluence.slac.stanford.edu/display/~ktkim/matpva%3A+Integrate+EPICS7+into+MATLAB+using+PVAccess+for+Python+%28P4P%29+module
