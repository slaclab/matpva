%% Matlab script to test mpvaGet function for the NTTable PVs in prod

% -----------------------------------------------------------------------------
% Title      : mpvaGet test script prod
% -----------------------------------------------------------------------------
% File       : mpvaGet_test_script_prod.m
% Author     : Kuktae Kim, ktkim@slac.stanford.edu
% Created    : 2023-11-02
% Last update: 2023-11-02
% -----------------------------------------------------------------------------
% Description:
% Matlab script to test mpvaGet function in production.
% -----------------------------------------------------------------------------
% This file is part of matpva. It is subject to the license terms in the 
% LICENSE.txt file found in the top-level directory of this distribution
% and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
% No part of matpva, including this file, may be copied, modified, 
% propagated, or distributed except according to the terms contained in 
% the LICENSE.txt file.
% -----------------------------------------------------------------------------

[NTTable, ts, alarm, NTStruct] =  mpvaGet("BLEM:SYS0:1:CU_HXR:LIVE:TWISS")
[NTTable, ts, alarm, NTStruct] =  mpvaGet("BLEM:SYS0:1:CU_SXR:EXTANT:TWISS")
[NTTable, ts, alarm, NTStruct] =  mpvaGet("BLEM:SYS0:1:SC_HXR:EXTANT:RMAT")
[NTTable, ts, alarm, NTStruct] =  mpvaGet("BLEM:SYS0:1:SC_BSYD:LIVE:TWISS")