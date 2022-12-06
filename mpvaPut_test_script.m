% Matlab script to test mpvaPut.m
% MatP4P = py.p4p.client.thread.Context('pva');

% Initial Scalar Values
IntValue = 0;
FloatValue = 0.7;
BoolValue = false;
ByteValue = 1;
ShortValue = 5;
StringValue = "empty string";

% % Test Scalar Values
IntValue = 3;
FloatValue = 7.7;
BoolValue = true;
ByteValue = 123;
ShortValue = 35;
StringValue = "test string";

% Test Scalar PVs
mpvaPut("TEST:PVA:IntValue", IntValue)
mpvaPut("TEST:PVA:FloatValue", FloatValue)
mpvaPut("TEST:PVA:BoolValue", BoolValue)
mpvaPut("TEST:PVA:ByteValue", ByteValue)
mpvaPut("TEST:PVA:ShortValue", ShortValue)
mpvaPut("TEST:PVA:StringValue", StringValue)



% Initial Array Values
ai = [0, 0, 0, 0, 0];
ah = [1, 1, 1];
af = [0.1, 0.2, 0.3, 0.4];
ad = [0.0, 0.0, 0.0, 0.0];
ab = [false, true, false, false];
as = ["Zero", "Zero", "Zero"];
% 
% Test Array Values
ai = [1, 2, 3, 4, 5, 6];
ah = [0, 1, 1, 0, 0, 0, 1, 1, 0];
af = [1.5, 2.5, 3.5, 4.5, 5.5, 6.7];
ad = [11.5, 22.5, 33.5, 44.5, 55.5, 66.5];
ab = [true, false, true, false, true];
as = ["One", "Two", "Three", "Four", "Five"];

% Test Array PVs
mpvaPut("TEST:PVA:IntArray", ai)
mpvaPut("TEST:PVA:ShortArray", ah)
mpvaPut("TEST:PVA:DoubleArray", ad)
mpvaPut("TEST:PVA:FloatArray", af)
mpvaPut("TEST:PVA:BoolArray", ab)
mpvaPut("TEST:PVA:StringArray", as)



% Initial NTTable Values
Name = ["device_1", "device_2"];
Number = [1.1, 1.2];

% Test NTTable Values
Name = ["test1","test2","test3","test4"];
Number = [1,2,3,4];
% 
% Test NTTable PVs
mpvaPut("TEST:PVA:NTTable", "name", Name, "number", Number)
mpvaPut("TEST:PVA:NTTable", "name", Name, "number", Number, "mpvaDebugOn")



% Initial TWISS NTTable Values
Element = ["SOL9000", "XC99", "YC99"]; Device_name = ["SOL:IN20:111", "XCOR:IN20:112", "YCOR:IN20:113"];
S = [1,2,3]; Z = [10,11,12]; Length = [100, 200, 300]; P0C = [6, 16, 26]; Alpha_x = [0.1, 0.2, 0.3];
Beta_x = [0.1, 0.2, 0.3]; Eta_x = [0.1, 0.2, 0.3]; Etap_x = [0.1, 0.2, 0.3]; Psi_x = [0.1, 0.2, 0.3];
Alpha_y = [0.1, 0.2, 0.3]; Beta_y = [0.1, 0.2, 0.3]; Eta_y = [0.1, 0.2, 0.3]; Etap_y = [0.1, 0.2, 0.3];
Psi_y = [0.1, 0.2, 0.3];

% Test TWISS NTTable Values
Element = ["SOL9000", "XC99", "YC99", "ZC99"]; Device_name = ["SOL:IN20:111", "XCOR:IN20:112", "YCOR:IN20:113", "ZCOR:IN20:114"];
S = [1, 2, 3, 4]; Z = [10 ,11, 12, 13]; Length = [100, 200, 300, 400]; P0C = [6, 16, 26, 36]; 
Alpha_x =[0.1, 0.2, 0.3, 0.4]; Beta_x = [0.1, 0.2, 0.3, 0.4]; Eta_x = [0.1, 0.2, 0.3, 0.4];
Etap_x = [0.1, 0.2, 0.3, 0.4]; Psi_x = [0.1, 0.2, 0.3, 0.4]; Alpha_y = [0.1, 0.2, 0.3, 0.4];
Beta_y = [0.1, 0.2, 0.3, 0.4]; Eta_y = [0.1, 0.2, 0.3, 0.4]; Etap_y = [0.1, 0.2, 0.3, 0.4];
Psi_y = [0.1, 0.2, 0.3, 0.4];


% Test TWISS NTTable PVs
mpvaPut("TBLEM:SYS0:1:CU_HXR:LIVE:TWISS", "element", Element, "device_name", Device_name, "s", S, "z", Z, "length", Length,...
    "p0c", P0C, "alpha_x", Alpha_x, "beta_x", Beta_x, "eta_x", Eta_x, "etap_x", Etap_x, "psi_x", Psi_x, "alpha_y", Alpha_y,...
    "beta_y", Beta_y, "eta_y", Eta_y, "etap_y", Etap_y, "psi_y", Psi_y)

mpvaPut("TBLEM:SYS0:1:CU_HXR:LIVE:TWISS", "element", Element, "device_name", Device_name, "s", S, "z", Z, "length", Length,...
    "p0c", P0C, "alpha_x", Alpha_x, "beta_x", Beta_x, "eta_x", Eta_x, "etap_x", Etap_x, "psi_x", Psi_x, "alpha_y", Alpha_y,...
    "beta_y", Beta_y, "eta_y", Eta_y, "etap_y", Etap_y, "psi_y", Psi_y, "mpvaDebugOn")





