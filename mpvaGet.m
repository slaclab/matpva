function varargout = mpvaGet(pvname)
%
% mpvaGet returns the values of given EPICS PV names.
%
%    [PV, ts, alarm] = mpvaGet(pvname)                       When PV is NTScalar or NTScalarArray type  
%    [NTTable, ts, alarm, NTStruct] = mpvaGet(pvname)        When PV is NTTable type
%

% -----------------------------------------------------------------------------
% Title      : mpvaGet
% -----------------------------------------------------------------------------
% File       : mpvaGet.m
% Author     : Kuktae Kim, ktkim@slac.stanford.edu
% Created    : 2023-11-02
% Last update: 2023-11-02
% -----------------------------------------------------------------------------
% Description:
% Return the values of given EPICS PV names.
% -----------------------------------------------------------------------------
% This file is part of matpva. It is subject to the license terms in the 
% LICENSE.txt file found in the top-level directory of this distribution
% and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
% No part of matpva, including this file, may be copied, modified, 
% propagated, or distributed except according to the terms contained in 
% the LICENSE.txt file.
% -----------------------------------------------------------------------------
if ~nargin
    error("Not enough input arguments. Please type an input PV.")
else
end

% Check if the input is valid PV
if (class(pvname) == "char") || (class(pvname) == "string")
    % pvname is valied no action is required.   
else
    msg = 'Please check if pvname is valid or the PV is alive';
    error(msg)
end


% Bring P4P python module into Matlab
MatP4P = py.p4p.client.thread.Context('pva', pyargs('nt', false));

PV = MatP4P.get(pvname);

% Check the ID of PV
nt_id = string(getID(PV));

% Check the type of PV
t = struct(py.dict(type(PV))).value;

% To have timeStamp information in human readable form
TimeInSeconds = double(int64(struct(struct(todict(PV)).timeStamp).secondsPastEpoch));
% NanoSec       = double(int64(struct(struct(todict(PV)).timeStamp).nanoseconds))/10^9;
% P4P time is based on 1970-01-01 and California time is GMT-8 
ts = datetime(TimeInSeconds, 'ConvertFrom', 'epochtime', 'Epoch', '1970-01-01', ...
    'Format', 'MMM dd, yyyy HH:mm:ss.SSS', 'TimeZone', 'UTC');
% Set your timezone
ts.TimeZone = 'America/Los_Angeles';

% To have alarm information
alarm.severity = int32(int64(struct(struct(todict(PV)).alarm).severity));
alarm.status   = int32(int64(struct(struct(todict(PV)).alarm).status));
alarm.message  = string(struct(struct(todict(PV)).alarm).message);

% NTScalarArrays data type PVs
if (contains(nt_id, "NTScalarArray"))
    if (t == "ai")
        ret = int32(py.array.array('i', struct(todict(PV)).value));
    elseif (t == "aI")
        ret = uint32(py.array.array('I', struct(todict(PV)).value));
    elseif (t == "ab")
        ret = int8(py.array.array('b', struct(todict(PV)).value));
    elseif (t == "aB")
        ret = uint8(py.array.array('B', struct(todict(PV)).value));
    elseif (t == "ah")
        ret = int16(py.array.array('h', struct(todict(PV)).value));
    elseif (t == "aH")
        ret = uint16(py.array.array('H', struct(todict(PV)).value));
    elseif (t == "al")
        ret = int64(py.array.array('l', struct(todict(PV)).value));
    elseif (t == "aL")
        ret = uint64(py.array.array('L', struct(todict(PV)).value));
    elseif (t == "af")
        ret = single(py.array.array('f', struct(todict(PV)).value));
    elseif (t == "ad")
        ret = double(py.array.array('d', struct(todict(PV)).value));
    elseif (t == "a?")
        ret = string(logical(py.array.array('b', struct(todict(PV)).value)));
    elseif (t == "as")
        ret = string(cell(struct(todict(PV)).value));
    else
    end
    varargout{1} = ret;
    varargout{2} = ts;
    varargout{3} = alarm;

% NTScalar data type PVs
elseif (contains(nt_id, "NTScalar"))
    if (t == 'i')
        ret = int32(int64(struct(todict(PV)).value));
    elseif (t == 'I')
        ret = uint32(int64(struct(todict(PV)).value));
    elseif (t == 'b')
        ret = int8(int64(struct(todict(PV)).value));
    elseif (t == 'B')
        ret = uint8(int64(struct(todict(PV)).value));
    elseif (t == 'h')
        ret = int16(int64(struct(todict(PV)).value));
    elseif (t == 'H')
        ret = uint16(int64(struct(todict(PV)).value));
    elseif (t == 'l')
        ret = int64(int64(struct(todict(PV)).value));
    elseif (t == 'L')
        ret = uint64(int64(struct(todict(PV)).value));
    elseif (t == '?')
        ret = string(struct(todict(PV)).value);
    elseif (t == 's')
        ret = string(py.str(struct(todict(PV)).value));
    elseif (t == 'f')
        ret = single(struct(todict(PV)).value);
    elseif (t == 'd')
        ret = struct(todict(PV)).value;
    else
    end
    varargout{1} = ret;
    varargout{2} = ts;
    varargout{3} = alarm;

% NTTable data type PVs    
elseif (contains(nt_id, "NTTable"))
    a           = struct(todict(PV)).labels;
    aa          = string(cell(a));
    numElement  = numel(aa);
    
    type_struct = struct(py.dict(struct(py.dict(type(PV))).value));
    val_struct  = struct(struct(todict(PV)).value);
    
    for iElement = 1 : numElement
        e_type = type_struct.(aa(iElement));
        v      = val_struct.(aa(iElement));
                        
        if (e_type == "ai")
            vv  = int32(py.array.array('i', v));
            vv2 = int32(py.array.array('i', v))';
        elseif (e_type == "aI")
            vv  = uint32(py.array.array('I', v));
            vv2 = uint32(py.array.array('I', v))';
        elseif (e_type == "ab")
            vv  = int8(py.array.array('b', v));
            vv2 = int8(py.array.array('b', v))';
        elseif (e_type == "aB")
            vv  = uint8(py.array.array('B', v));
            vv2 = uint8(py.array.array('B', v))';
        elseif (e_type == "ah")
            vv  = int16(py.array.array('h', v));
            vv2 = int16(py.array.array('h', v))';
        elseif (e_type == "aH")
            vv  = uint16(py.array.array('H', v));
            vv2 = uint16(py.array.array('H', v))';
        elseif (e_type == "al")
            vv  = int64(py.array.array('l', v));
            vv2 = int64(py.array.array('l', v))';
        elseif (e_type == "aL")
            vv  = uint64(py.array.array('L', v));
            vv2 = uint64(py.array.array('L', v))';
        elseif (e_type == "af")
            vv  = single(py.array.array('f', v));
            vv2 = single(py.array.array('f', v));
        elseif (e_type == "ad")
            vv  = double(py.array.array('d', v));
            vv2 = double(py.array.array('d', v))';
        elseif (e_type == "a?")
            vv  = string(logical(py.array.array('b', v)));
            vv2 = string(logical(py.array.array('b', v)))';
        elseif (e_type == "as")
            vv  = string(cell(v));
            vv2 = string(cell(v))';
        else
        end
        
        % Old method
        % eval("NTStruct." + aa(iElement) + "= vv;");
        % eval("NTStruct2." + aa(iElement) + "= vv2;");
        
        NTStruct.(aa(iElement)) = vv;
        % To make NTTable output with each element data in a column instead of in a row        
        NTStruct2.(aa(iElement)) = vv2;
    end
    
    NTTable      = struct2table(NTStruct2);
    varargout{1} = NTTable;
    varargout{2} = ts;
    varargout{3} = alarm;
    varargout{4} = NTStruct;

% NTEnum data type PVs    
elseif (contains(nt_id, "NTEnum"))
    idx     = double(struct(struct(todict(PV)).value).index);
    choice  = string(cell(struct(struct(todict(PV)).value).choices));
    % Convert Python style indexing to Matlab style indexing
    ret     = choice(idx + 1);
    
    varargout{1} = ret;
    varargout{2} = ts;
    varargout{3} = alarm;
    
else
    fprintf("This PV data type is not supported yet.");
end

MatP4P.close();

end

