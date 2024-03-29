%% Matlab script to test mpvaPut function for the NTTable PVs in prod

% -----------------------------------------------------------------------------
% Title      : mpvaPut test script prod
% -----------------------------------------------------------------------------
% File       : mpvaPut_test_script_prod.m
% Author     : Kuktae Kim, ktkim@slac.stanford.edu
% Created    : 2023-11-02
% Last update: 2023-11-02
% -----------------------------------------------------------------------------
% Description:
% Matlab script to test mpvaPut function in production.
% -----------------------------------------------------------------------------
% This file is part of matpva. It is subject to the license terms in the 
% LICENSE.txt file found in the top-level directory of this distribution
% and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
% No part of matpva, including this file, may be copied, modified, 
% propagated, or distributed except according to the terms contained in 
% the LICENSE.txt file.
% -----------------------------------------------------------------------------
% Test TWISS NTTable PV Values 1
Element     = ["SOL9000", "XC99", "YC99"];
DeviceName  = ["SOL:IN20:111", "XCOR:IN20:112", "YCOR:IN20:113"];
S           = [1, 2, 3];
Z           = [10, 11, 12];
Length      = [100, 200, 300];
P0C         = [6, 16, 26];
Alpha_x     = [0.1, 0.2, 0.3];
Beta_x      = [0.1, 0.2, 0.3];
Eta_x       = [0.1, 0.2, 0.3];
Etap_x      = [0.1, 0.2, 0.3];
Psi_x       = [0.1, 0.2, 0.3];
Alpha_y     = [0.1, 0.2, 0.3];
Beta_y      = [0.1, 0.2, 0.3];
Eta_y       = [0.1, 0.2, 0.3];
Etap_y      = [0.1, 0.2, 0.3];
Psi_y       = [0.1, 0.2, 0.3];

% Test TWISS NTTable PV Values 2
Element2    = ["SOL9000", "XC99", "YC99", "ZC99"];
DeviceName2 = ["SOL:IN20:111", "XCOR:IN20:112", "YCOR:IN20:113", "ZCOR:IN20:114"];
S2          = [1, 2, 3, 4];
Z2          = [10, 11, 12, 13];
Length2     = [100, 200, 300, 400];
P0C2        = [6, 16, 26, 36];
Alpha_x2    = [0.1, 0.2, 0.3, 0.4];
Beta_x2     = [0.1, 0.2, 0.3, 0.4];
Eta_x2      = [0.1, 0.2, 0.3, 0.4];
Etap_x2     = [0.1, 0.2, 0.3, 0.4];
Psi_x2      = [0.1, 0.2, 0.3, 0.4];
Alpha_y2    = [0.1, 0.2, 0.3, 0.4];
Beta_y2     = [0.1, 0.2, 0.3, 0.4];
Eta_y2      = [0.1, 0.2, 0.3, 0.4];
Etap_y2     = [0.1, 0.2, 0.3, 0.4];
Psi_y2      = [0.1, 0.2, 0.3, 0.4];


% Put Test TWISS NTTable PV Values
mpvaPut("BLEM:SYS0:1:CU_HXR:LIVE:TWISS", "element", Element, "device_name", DeviceName, "s", S, ...
    "z", Z, "length", Length, "p0c", P0C, "alpha_x", Alpha_x, "beta_x", Beta_x, "eta_x", Eta_x, ...
    "etap_x", Etap_x, "psi_x", Psi_x, "alpha_y", Alpha_y, "beta_y", Beta_y, "eta_y", Eta_y, ...
    "etap_y", Etap_y, "psi_y", Psi_y, "mpvaDebugOn")

mpvaPut("BLEM:SYS0:1:SC_BSYD:LIVE:TWISS", "element", Element2, "device_name", DeviceName2, "s", S2, ...
    "z", Z2, "length", Length2, "p0c", P0C2, "alpha_x", Alpha_x2, "beta_x", Beta_x2, "eta_x", Eta_x2, ...
    "etap_x", Etap_x2, "psi_x", Psi_x2, "alpha_y", Alpha_y2, "beta_y", Beta_y2, "eta_y", ...
    Eta_y2, "etap_y", Etap_y2, "psi_y", Psi_y2, "mpvaDebugOn")





