% Matlab function for the pvget using the P4P pythone module  in the MATLAB 2020a
function varargout = MatP4Pget(pvname)
%% MatP4Pget returns the values of given EPICS PV names.
%
% INPUTS:
%    pvname               A PV name    
%

% Bring P4P python module into Matlab
% MatP4P = py.p4p.client.thread.Context('pva', pyargs('nt', false));
MatP4P = py.p4p.client.thread.Context('pva');

PV = MatP4P.get(pvname);

% Check the type of PV
t = class(PV);


% if (contains(nt_id, "NTScalarArray"))
% NTArray
if (t == "py.p4p.nt.scalar.ntnumericarray")
    if (PV.dtype == 'int32')
        ret = int32(py.array.array('i', PV));
    elseif (PV.dtype == 'int16')
        ret = int16(py.array.array('i', PV));
    elseif (PV.dtype == 'float32')
        ret = single(py.array.array('f', PV));
    elseif (PV.dtype == 'float64')
        ret = double(py.array.array('d', PV));
    elseif (PV.dtype == 'bool')
        ret = string(logical(py.array.array('b', PV)));     
    end
    varargout{1} = ret;
        
elseif (t == "py.p4p.nt.scalar.ntstringarray")
    ret = string(cell(PV)); 
    varargout{1} = ret;

% Scalar PVs
% elseif (contains(nt_id, "NTScalar"))
elseif (t == "py.p4p.nt.scalar.ntint")
    if (struct(py.dict(type(PV.raw))).value == 'i')
        ret = int32(int64(py.int(PV)));        
    elseif (struct(py.dict(type(PV.raw))).value == 'b')
        ret = int8(int64(py.int(PV))); 
    elseif (struct(py.dict(type(PV.raw))).value == 'h')
        ret = int16(int64(py.int(PV)));
    elseif (struct(py.dict(type(PV.raw))).value == 'l')
        ret = int64(int64(py.int(PV)));
    else
    end
    varargout{1} = ret;
    
elseif (t == "py.p4p.nt.scalar.ntbool")
    ret = string(struct(py.dict(PV.raw)).value);
    varargout{1} = ret;
    
elseif (t == "py.p4p.nt.scalar.ntstr")
    ret = string(py.str(struct(py.dict(PV.raw)).value));
    varargout{1} = ret;
    
elseif (t == "double")
    MatP4P = py.p4p.client.thread.Context('pva', pyargs('nt', false));
    PV = MatP4P.get(pvname);
    if (struct(py.dict(type(PV))).value) == 'f'
        ret = single(struct(py.dict(PV)).value);
    else
        ret = (struct(py.dict(PV)).value);        
    end
    varargout{1} = ret;

% NTTable
elseif (t== "py.p4p.wrapper.Value")
    % Check the ID of PV
    nt_id = string(getID(PV));        
    
    if (contains(nt_id, "NTTable"))
        a = struct(todict(PV)).labels;
        aa = string(cell(a));
        k=numel(aa);
        
        for i=1:k            
            t_str = "struct(py.dict(struct(py.dict(type(PV))).value))." + aa(i);
            str = "struct(struct(todict(PV)).value)." + aa(i);                 
            e_type = eval(t_str);
            v = eval(str);
            
            if (e_type == 'ai')
                vv = int32(py.array.array('i', v));
            elseif (e_type == 'af')
                vv = single(py.array.array('f', v));
            elseif (e_type == 'ad')
                vv = double(py.array.array('d', v));
            elseif (e_type == 'a?')
                vv = string(logical(py.array.array('b', v)));
            elseif (e_type == 'as')
                vv = string(cell(v));
            end                

            eval("NTStruct." + aa(i) + "= vv;");
        end
        ret = NTStruct;
        NTTable = struct2table(NTStruct);
        
        % timeStamp
        TimeInSeconds = double(int64(struct(struct(todict(PV)).timeStamp).secondsPastEpoch));
        NanoSec = double(int64(struct(struct(todict(PV)).timeStamp).nanoseconds))/10^9;
        ts=datetime(TimeInSeconds-8*3600+NanoSec, 'ConvertFrom', 'epochtime', 'Epoch', '1970-01-01', 'Format', 'MMM dd, yyyy HH:mm:ss.SSS') 

        
        % alarm
        alarm.severity = int32(int64(struct(struct(todict(PV)).alarm).severity));
        alarm.status = int32(int64(struct(struct(todict(PV)).alarm).status));
        alarm.message = string(struct(struct(todict(PV)).alarm).message);
                
        varargout{1} = NTStruct;
        varargout{2} = NTTable;
        varargout{3} = ts;
        varargout{4} = alarm;
        
    else
        fprintf("This PV data type is not supported yet.");
    end
else
    fprintf("This PV data type is not supported yet.");
end
