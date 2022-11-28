% command = 'pvput SIOC:B15:RF03:0:DMOD_LABEL3 15
% command = 'pvput BLEM:SYS0:1:CU_HXR:LIVE:TWISS value.element="[SOL9000, XC99, YC99]" value.device_name="[\"SOL:IN20:111\", \"XCOR:IN20:112\", \"YCOR:IN20:113\"]" value.s="[1,2,3]" value.z="[10, 11, 12]" value.length="[100, 200, 300]" value.p0c="[6, 16, 26]" value.alpha_x="[0.1, 0.2, 0.3]" value.beta_x="[0.1, 0.2, 0.3]" value.eta_x="[0.1, 0.2, 0.3]" value.etap_x="[0.1, 0.2, 0.3]" value.psi_x="[0.1, 0.2, 0.3]" value.alpha_y="[0.1, 0.2, 0.3]" value.beta_y="[0.1, 0.2, 0.3]" value.eta_y="[0.1, 0.2, 0.3]" value.etap_y="[0.1, 0.2, 0.3]" value.psi_y="[0.1, 0.2, 0.3]"'
% command = 'pvget KTKIM:TBL'
% command = "pvput KTKIM:TBL value.name='[test1, test2]' value.number='[10,20]'"
% command = 'pvput KTKIM:TBL value.name="[device_1, device_2]" value.number="[1.1,1.2]"'
% command = "pvput KTKIM:TBL value.name=""[test1, test2, test3]"" value.number=""[10,20,30]"""

% line1 = 'pvput BLEM:SYS0:1:CU_HXR:LIVE:TWISS ';
% line2 = 'value.element="[SOL9000, XC99, YC99]" ';
% line3 = 'value.device_name="[\"SOL:IN20:111\", \"XCOR:IN20:112\", \"YCOR:IN20:113\"]" ';
% line4 = 'value.s="[1,2,3]" value.z="[10, 11, 12]" value.length="[100, 200, 300]" ';
% line5 = 'value.p0c="[6, 16, 26]" value.alpha_x="[0.1, 0.2, 0.3]" value.beta_x="[0.1, 0.2, 0.3]" ';
% line6 = 'value.eta_x="[0.1, 0.2, 0.3]" value.etap_x="[0.1, 0.2, 0.3]" value.psi_x="[0.1, 0.2, 0.3]" ';
% line7 = 'value.alpha_y="[0.1, 0.2, 0.3]" value.beta_y="[0.1, 0.2, 0.3]" value.eta_y="[0.1, 0.2, 0.3]" ';
% line8 = 'value.etap_y="[0.1, 0.2, 0.3]" value.psi_y="[0.1, 0.2, 0.3]" ';

line1 = "pvput BLEM:SYS0:1:CU_HXR:LIVE:TWISS ";
line2 = "value.element=""[%s, %s, %s]"" ";
line3 = "value.device_name=""[%s, %s, %s]"" ";
line4 = "value.s=""[%d, %d, %d]"" ";
line5 = "value.z=""[%d, %d, %d]"" ";
line6 = "value.length=""[%d, %d, %d]"" ";
line7 = "value.p0c=""[%d, %d, %d]"" ";
line8 = "value.alpha_x=""[%f, %f, %f]"" ";
line9 = "value.beta_x=""[%f, %f, %f]"" ";
line10 = "value.eta_x=""[%f, %f, %f]"" ";
line11 = "value.etap_x=""[%f, %f, %f]"" ";
line12 = "value.psi_x=""[%f, %f, %f]"" ";
line13 = "value.alpha_y=""[%f, %f, %f]"" ";
line14 = "value.beta_y=""[%f, %f, %f]"" ";
line15 = "value.eta_y=""[%f, %f, %f]"" ";
line16 = "value.etap_y=""[%f, %f, %f]"" ";
line17 = "value.psi_y=""[%f, %f, %f]""";

lines= line1 + line2 + line3 + line4 + line5 + line6 + line7 + line8 +line9 ...
    + line10 +line11 + line12 + line13 + line14 + line15 + line16 + line17;

A=["SOL9000", "XC99", "YC99"]; B=["\""SOL:IN20:111\""", "\""XCOR:IN20:112\""", "\""YCOR:IN20:113\"""]; C=[1, 2, 3]; D=[10, 11, 12];
E=[100, 200, 300]; F=[6, 16, 26]; G=[0.1, 0.2, 0.3]; H=[0.1, 0.2, 0.3]; I=[0.1, 0.2, 0.3]; J=[0.1, 0.2, 0.3];
K=[0.1, 0.2, 0.3]; L=[0.1, 0.2, 0.3]; M=[0.1, 0.2, 0.3]; N=[0.1, 0.2, 0.3]; O=[0.1, 0.2, 0.3]; P=[0.1, 0.2, 0.3];


command = compose(lines, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P)

[status, cmdout]=system(command)

