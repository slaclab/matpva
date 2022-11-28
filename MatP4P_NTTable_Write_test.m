% Matlab script to writie NTTable PV in Matlab
MatP4P = py.p4p.client.thread.Context('pva');

% MatP4P.get('TEST:SCALAR')
% MatP4P.put('TEST:SCALAR', pyargs('values','test'))
% MatP4P.get('TEST:SCALAR')
% 
%  
% MatP4P.get('TEST:SCALAR')
% MatP4P.put('TEST:SCALAR', struct('value', 'what'))
% MatP4P.get('TEST:SCALAR')
% 
% MatP4P.get('TEST:SCALAR')
% MatP4P.put('TEST:SCALAR', py.dict(pyargs('value', 'test2')))
% MatP4P.get('TEST:SCALAR')

% % NTNumericStringArray 
% MatP4P.get('TEST:ARRAYs')
% MatP4P.put('TEST:ARRAYs', struct(py.dict(pyargs('value', [{'test1'}, {'test2'}]))))
% MatP4P.get('TEST:ARRAYs')
% MatP4P.put('TEST:ARRAYs', ([{'test1'}, {'test2'}, {'test3'}]))
% MatP4P.get('TEST:ARRAYs')

% % NTNumericArray Float
% MatP4P.get('TEST:ARRAYd')
% MatP4P.put('TEST:ARRAYd', struct(py.dict(pyargs('value',[1.9, 2.2, 3.7, 4.5]))))
% MatP4P.get('TEST:ARRAYd')
% MatP4P.put('TEST:ARRAYd', struct(py.dict(pyargs('value',[0.1, 0.2, 0.3, 0.4]))))
% MatP4P.get('TEST:ARRAYd')

% % NTNumericArray Integer
% MatP4P.get('TEST:ARRAYi')
% % MatP4P.put('TEST:ARRAYi', struct(py.dict(pyargs('value',int32([9, 8, 7, 6,5])))))
% MatP4P.put('TEST:ARRAYi', [{9}, {8}, {7}, {6}, {5}])
% MatP4P.get('TEST:ARRAYi')
% % MatP4P.put('TEST:ARRAYi', struct(py.dict(pyargs('value',int32([0, 0, 0, 0])))))
% MatP4P.put('TEST:ARRAYi', [{0}, {0}, {0}, {0}])
% MatP4P.get('TEST:ARRAYi')

% % NTNumericArray Bool
% MatP4P.get('TEST:PVA:BoolArray')
% MatP4P.put('TEST:PVA:BoolArray', [{false}, {false}, {false}, {false}, {false}])
% MatP4P.get('TEST:PVA:BoolArray')
% MatP4P.put('TEST:PVA:BoolArray', [{true}, {false}, {true}])
% MatP4P.get('TEST:PVA:BoolArray')

% MatP4P.get('TEST:TBL')
% 
% a=MatP4P.get('TEST:TBL')
% b=struct(py.dict(a))
% b.labels
% 
% % struct(struct(a.todict).value).name
% MatP4P.get('TEST:TBL');
% MatP4P.put('TEST:TBL', struct(py.dict(pyargs('value', py.dict(pyargs('name', [{'test1'},{'test2'},{'test3'},{'test4'}],'number',[1,2,3,4]))))))
% MatP4P.get('TEST:TBL')


% % Initial NTTable Values
% Name = ["device_1", "device_2"]
% Number = [1.1, 1.2]

% Test NTTable Values
Name = (["test1","test2","test3","test4"]);
Number = ([1,2,3,4]);

k=numel(Name)

data = num2cell(["name", Name, "number", Number])
data1 = (["name", Name, "number", Number])

data2 = [data{1},{data{1+1:1+1+k-1}},data{k+2},{data{k+2+1:k+3+k-1}}]

data3 = "pyargs(data{1}, {data{1+1:1+1+k-1}}, data{k+2}, {data{k+2+1:k+3+k-1}})"
data4 = "pyargs(""name"", num2cell(Name), ""number"", num2cell(Number))"

% NTTable Test
MatP4P.get('TEST:PVA:NTTable')
% MatP4P.put('TEST:PVA:NTTable', struct(py.dict(pyargs('value', py.dict(pyargs("name", Name,"number", Number))))))
% MatP4P.put('TEST:PVA:NTTable', py.dict(pyargs('value', py.dict(pyargs(data{1}, {data{1+1:1+1+k-1}}, data{k+2}, {data{k+2+1:k+3+k-1}})))))
MatP4P.put('TEST:PVA:NTTable', py.dict(pyargs('value', py.dict(eval(data4)))))
MatP4P.get('TEST:PVA:NTTable')



% Reuse NTStruct MATLAB variables.
[~, NTStruct, ~, ~] = MatP4Pget("TEST:PVA:NTTable")
fileds = fieldnames(NTStruct)
data = "pyargs(fields{1}, num2cell(NTStruct.name), fields{2}, num2cell(NTStruct.number))"
MatP4P.put('TEST:PVA:NTTable', py.dict(pyargs('value', py.dict(eval(data)))))

% Reuse NTTable MATLAB variables.
[NTTable, ~, ~, ~] = MatP4Pget("TEST:PVA:NTTable")
fileds = NTTable.Properties.VariableNames
data = "pyargs(fields{1}, num2cell(NTTable.name'), fields{2}, num2cell(NTTable.number'))"
MatP4P.put('TEST:PVA:NTTable', py.dict(pyargs('value', py.dict(eval(data)))))



% 
% MatP4P.get('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS')
% MatP4P.put('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS', struct(py.dict(pyargs('value', py.dict(pyargs('element', [{'SOL9000'}, {'XC99'}, {'YC99'}],...
%     'device_name',[{'SOL:IN20:111'}, {'XCOR:IN20:112'}, {'YCOR:IN20:113'}],'s',[1,2,3],'z',[10,11,12],'length',[100, 200, 300],...
%     'p0c',[6, 16, 26], 'alpha_x',[0.1, 0.2, 0.3], 'beta_x',[0.1, 0.2, 0.3], 'eta_x',[0.1, 0.2, 0.3], 'etap_x',[0.1, 0.2, 0.3], 'psi_x',[0.1, 0.2, 0.3],...
%     'alpha_y',[0.1, 0.2, 0.3], 'beta_y',[0.1, 0.2, 0.3], 'eta_y',[0.1, 0.2, 0.3], 'etap_y',[0.1, 0.2, 0.3], 'psi_y',[0.1, 0.2, 0.3]))))))
% MatP4P.get('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS')



% MatP4P.get('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS')
% MatP4P.put('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS', struct(py.dict(pyargs('value', py.dict(pyargs('element', [{'SOL9000'}, {'XC99'}, {'YC99'}, {'ZC99'}],...
%     'device_name',[{'SOL:IN20:111'}, {'XCOR:IN20:112'}, {'YCOR:IN20:113'}, {'ZCOR:IN20:114'}],'s',[1, 2, 3, 4],'z',[10 ,11, 12, 13],'length',[100, 200, 300, 400],...
%     'p0c',[6, 16, 26, 36], 'alpha_x',[0.1, 0.2, 0.3, 0.4], 'beta_x',[0.1, 0.2, 0.3, 0.4], 'eta_x',[0.1, 0.2, 0.3, 0.4], 'etap_x',[0.1, 0.2, 0.3, 0.4], 'psi_x',[0.1, 0.2, 0.3, 0.4],...
%     'alpha_y',[0.1, 0.2, 0.3, 0.4], 'beta_y',[0.1, 0.2, 0.3, 0.4], 'eta_y',[0.1, 0.2, 0.3, 0.4], 'etap_y',[0.1, 0.2, 0.3, 0.4], 'psi_y',[0.1, 0.2, 0.3, 0.4]))))))
% MatP4P.get('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS')


