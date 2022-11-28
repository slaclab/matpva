% Matlab script to understand type of PVs using the P4P pythone module in the MATLAB 2020a
MatP4P = py.p4p.client.thread.Context('pva');

IntValue = class(MatP4P.get('TEST:PVA:IntValue'))

FloatValue = class(MatP4P.get('TEST:PVA:FloatValue'))

DoubleValue = class(MatP4P.get('TEST:PVA:DoubleValue'))

BoolValue = class(MatP4P.get('TEST:PVA:BoolValue'))

ByteValue = class(MatP4P.get('TEST:PVA:ByteValue'))

ShortValue = class(MatP4P.get('TEST:PVA:ShortValue'))

StringValue = class(MatP4P.get('TEST:PVA:StringValue'))

IntArray = class(MatP4P.get('TEST:PVA:IntArray'))

ShortArray = class(MatP4P.get('TEST:PVA:ShortArray'))

FloatArray = class(MatP4P.get('TEST:PVA:FloatArray'))

DoubleArray = class(MatP4P.get('TEST:PVA:DoubleArray'))

Waveform = class(MatP4P.get('TEST:PVA:Waveform'))

BoolArray = class(MatP4P.get('TEST:PVA:BoolArray'))

StringArray = class(MatP4P.get('TEST:PVA:StringArray'))

Image = class(MatP4P.get('TEST:PVA:Image'))

NTTable = class(MatP4P.get('TEST:PVA:NTTable'))

MatP4P.get('TEST:PVA:NTTable')