% Matlab script to use the P4P pythone module for the pvget in the MATLAB 2020a
MatP4P = py.p4p.client.thread.Context('pva');

x=MatP4P.get('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS');

% Extract each elements from the NTTable and conver to Matlab arrays
a = struct(todict(x)).labels; aa = string(cell(a));
v1 = struct(struct(todict(x)).value).element; v_1 = string(cell(v1));
v2 = struct(struct(todict(x)).value).device_name; v_2 = string(cell(v2));
v3 = struct(struct(todict(x)).value).s; v_3 = double(py.array.array('d', v3));
v4 = struct(struct(todict(x)).value).z; v_4 = double(py.array.array('d', v4));
v5 = struct(struct(todict(x)).value).length; v_5 = double(py.array.array('d', v5));
v6 = struct(struct(todict(x)).value).p0c; v_6 = double(py.array.array('d', v6));
v7 =  struct(struct(todict(x)).value).alpha_x; v_7 = double(py.array.array('d', v7));
v8 =  struct(struct(todict(x)).value).beta_x; v_8 = double(py.array.array('d', v8));
v9 =  struct(struct(todict(x)).value).eta_x; v_9 = double(py.array.array('d', v9));
v10 =  struct(struct(todict(x)).value).etap_x; v_10 = double(py.array.array('d', v10));
v11 =  struct(struct(todict(x)).value).psi_x; v_11 = double(py.array.array('d', v11));
v12 =  struct(struct(todict(x)).value).alpha_y; v_12 = double(py.array.array('d', v12));
v13 =  struct(struct(todict(x)).value).beta_y; v_13 = double(py.array.array('d', v13));
v14 =  struct(struct(todict(x)).value).eta_y; v_14 = double(py.array.array('d', v14));
v15 =  struct(struct(todict(x)).value).etap_y; v_15 = double(py.array.array('d', v15));
v16 =  struct(struct(todict(x)).value).psi_y; v_16 = double(py.array.array('d', v16));

% Fileds for the Matlab table
field1 = aa{1}; field2 = aa{2}; field3 = aa{3}; field4 = aa{4}; field5 = aa{5}; field6 = aa{6}; field7 = aa{7};
field8 = aa{8}; field9 = aa{9}; field10 = aa{10}; field11 = aa{11}; field12 = aa{12}; field13 = aa{13}; field14 = aa{14};
field15 = aa{15}; field16 = aa{16};

% Values for the each filed
value1 = v_1; value2 = v_2; value3 = v_3; value4 = v_4; value5 = v_5; value6 = v_6; value7 = v_7; value8 = v_8;
value9 = v_9; value10 = v_10; value11 = v_11; value12 = v_12; value13 = v_13; value14 = v_14; value15 = v_15; value16 = v_16;

% Make a Matlab struct (NTTstruct)
NTStruct = struct(field1, value1, field2, value2, field3, value3, field4, value4, field5, value5, field6, value6, field7, value7,...
field8, value8, field9, value9, field10, value10, field11, value11, field12, value12, field13, value13, field14, value14,...
field15, value15, field16, value16)

% Make a Matlab Table (NTTable)
NTTable = struct2table(NTstruct)