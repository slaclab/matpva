% Matlab script to test mpvaGet function for the NTTable PVs in prod
[NTTable, ts, alarm, NTStruct] =  mpvaGet("BLEM:SYS0:1:CU_HXR:LIVE:TWISS")
[NTTable, ts, alarm, NTStruct] =  mpvaGet("BLEM:SYS0:1:CU_SXR:EXTANT:TWISS")
[NTTable, ts, alarm, NTStruct] =  mpvaGet("BLEM:SYS0:1:SC_HXR:EXTANT:RMAT")
[NTTable, ts, alarm, NTStruct] =  mpvaGet("BLEM:SYS0:1:SC_BSYD:LIVE:TWISS")

