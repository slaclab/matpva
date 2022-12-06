% Matlab script to test mpvaGet function

% Scalar PVs
[PV, ts, alarm] = mpvaGet("TEST:PVA:IntValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:uIntValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:ByteValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:uByteValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:ShortValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:uShortValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:LongValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:uLongValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:FloatValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:DoubleValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:BoolValue")
[PV, ts, alarm] = mpvaGet("TEST:PVA:StringValue")

% Array PVs
[PV, ts, alarm] = mpvaGet("TEST:PVA:IntArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:uIntArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:ByteArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:uByteArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:ShortArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:uShortArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:LongArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:uLongArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:FloatArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:DoubleArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:Waveform")
[PV, ts, alarm] = mpvaGet("TEST:PVA:BoolArray")
[PV, ts, alarm] = mpvaGet("TEST:PVA:StringArray")

% NTNDArray
% Image = class(MatP4P.get('TEST:PVA:Image'))

% NTTable
[NTTable, ts, alarm, NTStruct] = mpvaGet("TEST:PVA:NTTable")

% NTTable TWISS
[NTTable, ts, alarm, NTStruct] =  mpvaGet("TBLEM:SYS0:1:CU_HXR:LIVE:TWISS")