%% Matlab script to test mpvaGet function

% -----------------------------------------------------------------------------
% Title      : mpvaGet test script
% -----------------------------------------------------------------------------
% File       : mpvaGet_test_script.m
% Author     : Kuktae Kim, ktkim@slac.stanford.edu
% Created    : 2023-11-02
% Last update: 2023-11-02
% -----------------------------------------------------------------------------
% Description:
% Matlab script to test mpvaGet function.
% -----------------------------------------------------------------------------
% This file is part of matpva. It is subject to the license terms in the 
% LICENSE.txt file found in the top-level directory of this distribution
% and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
% No part of matpva, including this file, may be copied, modified, 
% propagated, or distributed except according to the terms contained in 
% the LICENSE.txt file.
% -----------------------------------------------------------------------------

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

% NTNDArray PV
% Image = class(MatP4P.get('TEST:PVA:Image'))

% NTTable PV
[NTTable, ts, alarm, NTStruct] = mpvaGet("TEST:PVA:NTTable")

% NTTable TWISS PV
[NTTable, ts, alarm, NTStruct] =  mpvaGet("TBLEM:SYS0:1:CU_HXR:LIVE:TWISS")