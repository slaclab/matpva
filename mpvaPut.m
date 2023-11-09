function mpvaPut(pvname, varargin)
%
% mpvaPut puts the input values into the designated field in the given EPICS PV name.
%
%    mpvaPut(pvname, value)                                      NTScalar or NTScalarArray PVs
%    mpvaPut(pvname, field1, value1, field2, value2, ...)        NTTable PVs
%    mpvaPut(pvname, struct/table)                               NTTable PVs
%
% By default, mpvaPut doesn't display anything.
% If you would like to print out previous and updated PVs, add "mpvaDebugOn"
%
%    mpvaPut(pvname, value, "mpvaDebugOn")                                      NTScalar or NTScalarArray PVs
%    mpvaPut(pvname, field1, value1, field2, value2, ..., "mpvaDebugOn")        NTTable PVs
%    mpvaPut(pvname, struct/table,  "mpvaDebugOn")                              NTTable PVs
%

% -----------------------------------------------------------------------------
% Title      : mpvaPut
% -----------------------------------------------------------------------------
% File       : mpvaPut.m
% Author     : Kuktae Kim, ktkim@slac.stanford.edu
% Created    : 2023-11-02
% Last update: 2023-11-02
% -----------------------------------------------------------------------------
% Description:
% Put the input values into the designated field in the given EPICS PV name.
% -----------------------------------------------------------------------------
% This file is part of matpva. It is subject to the license terms in the 
% LICENSE.txt file found in the top-level directory of this distribution
% and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
% No part of matpva, including this file, may be copied, modified, 
% propagated, or distributed except according to the terms contained in 
% the LICENSE.txt file.
% -----------------------------------------------------------------------------
if nargin < 2
    error("Not enough input arguments. Please type help mpvaPut for the usage.")
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

% Old PV for debugging purpose
if (class(varargin{end}) == "string")
    if (varargin{end} == "mpvaDebugOn")
        fprintf('The old PV is');
        old_PV = mpvaGet(pvname)
    else
    end
else
end

PV = MatP4P.get(pvname);

% Check the ID of PV
nt_id = string(getID(PV));

% Check the type of PV
% t = class(PV);


% NTScalarArrays
if (contains(nt_id, "NTScalarArray"))
    data = num2cell(varargin{1});
    MatP4P.put(pvname, data);
   
% Scalar PVs    
elseif (contains(nt_id, "NTScalar"))
    MatP4P.put(pvname, varargin{1});

  
% NTTable PVs    
elseif (contains(nt_id, "NTTable"))
    c = class(varargin{1});
    
    % NTStruct inputs
    if (c == "struct")
        str     = "pyargs(";
        fields  = fieldnames(varargin{1});
        for iField = 1 : (numel(fields) - 1)
            str = str + "fields{" + iField + "}, " + "num2cell(varargin{1}." + fields(iField) + "), ";
        end
        str = str + "fields{" + numel(fields) + "}, " + "num2cell(varargin{1}." + fields(numel(fields)) + "))";
        MatP4P.put(pvname, py.dict(pyargs('value', py.dict(eval(str)))));
%         MatP4P.put(pvname, py.dict(pyargs('value', py.dict(str))));
        
    % NTTable inputs          
    elseif (c == "table")
        str     = "pyargs(";
        fields  = varargin{1}.Properties.VariableNames;
        for iElem = 1 : (numel(fields) - 1)
            str = str + "fields{" + iElem + "}, " + "num2cell(varargin{1}." + fields(iElem) + "'), ";
        end
        str = str + "fields{" + numel(fields) + "}, " + "num2cell(varargin{1}." + fields(numel(fields)) + "'))";
        MatP4P.put(pvname, py.dict(pyargs('value', py.dict(eval(str)))));
%         MatP4P.put(pvname, py.dict(pyargs('value', py.dict(str))));
        
    else
        if (string(varargin{end}) == "mpvaDebugOn")
            % number of input elements are total argument inputs - 2 
            % (one is PV name and the other is for the debugging mode indicator)
            num_ielements = nargin - 2;
        else
            % number of input elements are total argument inputs - 1 (one is PV name)
            num_ielements = nargin - 1;
        end
        
        str = "pyargs(";
        
        % total number of inputs - 1 (one is for the last inputs)
        for iElement = 1 : (num_ielements - 1)
            if (rem(iElement, 2) == 1)
                str = str + "varargin{" + iElement + "},";
            else
                str = str + "num2cell(varargin{" + iElement + "}),";
            end
        end
        
        str = str + "num2cell(varargin{" + (num_ielements) + "}))";
        MatP4P.put(pvname, py.dict(pyargs('value', py.dict(eval(str)))));       
    end
    
else
end

% Return the new PV value for debuggin purpose
if (class(varargin{end}) == "string")
    if (varargin{end} == "mpvaDebugOn")
        fprintf('The update PV is');
        updated_PV = mpvaGet(pvname)
    else
    end
else
end

MatP4P.close();
end
