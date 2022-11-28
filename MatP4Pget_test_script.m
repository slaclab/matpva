% Matlab script to test MatP4Pput.m
% MatP4P = py.p4p.client.thread.Context('pva');
% MatP4P = py.p4p.client.thread.Context('pva', pyargs('nt', false));

% Scalar PVs
[PV, ts, alarm] = MatP4Pget("TEST:PVA:IntValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:uIntValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:ByteValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:uByteValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:ShortValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:uShortValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:LongValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:uLongValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:FloatValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:DoubleValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:BoolValue")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:StringValue")

% Array PVs
[PV, ts, alarm] = MatP4Pget("TEST:PVA:IntArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:uIntArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:ByteArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:uByteArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:ShortArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:uShortArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:LongArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:uLongArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:FloatArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:DoubleArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:Waveform")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:BoolArray")
[PV, ts, alarm] = MatP4Pget("TEST:PVA:StringArray")

% NTNDArray
% Image = class(MatP4P.get('TEST:PVA:Image'))

% NTTable
[NTTable, NTStruct, ts, alarm] = MatP4Pget("TEST:PVA:NTTable")

% NTTable TWISS
[NTTable, NTStruct, ts, alarm] =  MatP4Pget("TBLEM:SYS0:1:CU_HXR:LIVE:TWISS")