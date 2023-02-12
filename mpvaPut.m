%% Matlab function for the pvput using the P4P pythone module  in the MATLAB 2020a
function mpvaPut(pvname, varargin)
%% mpvaPut displays the previous and updated values of given EPICS PV names.
%
% INPUTS:
%    pvname               A PV name
%    varargin             Inputs to put the values in the PV
%

% Bring P4P python module into Matlab
MatP4P = py.p4p.client.thread.Context('pva', pyargs('nt', false));

% Old PV for debugging purpose
if (class(varargin{end}) == "string")
    if (varargin{end} == "mpvaDebugOn")
        fprintf('The old PV is');
        old_PV = mpvaGet(pvname)
    end
end

PV = MatP4P.get(pvname);

% Check the ID of PV
nt_id = string(getID(PV));

% Check the type of PV
t = class(PV);


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
        str = "pyargs(";
        fields = fieldnames(varargin{1});
        for i=1:1:(numel(fields)-1)
            str = str + "fields{" + i + "}, " + "num2cell(varargin{1}." + fields(i) + "), ";
        end
        str = str + "fields{" + numel(fields) + "}, " + "num2cell(varargin{1}." + fields(numel(fields)) + "))";
        MatP4P.put(pvname, py.dict(pyargs('value', py.dict(eval(str)))));        
        
    % NTTable inputs          
    elseif (c == "table")
        str = "pyargs(";
        fields = varargin{1}.Properties.VariableNames;
        for i=1:1:(numel(fields)-1)
            str = str + "fields{" + i + "}, " + "num2cell(varargin{1}." + fields(i) + "'), ";
        end
        str = str + "fields{" + numel(fields) + "}, " + "num2cell(varargin{1}." + fields(numel(fields)) + "'))";
        MatP4P.put(pvname, py.dict(pyargs('value', py.dict(eval(str)))));         
        
    else
        if (string(varargin{end}) == "mpvaDebugOn")
            num_ielements = nargin - 2;     % number of input elements are total argument inputs - 2 (one is PV name and the other is for the debugging mode indicator)                       
        else
            num_ielements = nargin - 1;     % number of input elements are total argument inputs - 1 (one is PV name)
        end
        
        str = "pyargs(";
        
        for i=1:1:(num_ielements-1)    % total number of inputs - 1 (one is for the last inputs)
            if (rem(i,2)==1)
                str = str + "varargin{" + i + "},";
            else
                str = str + "num2cell(varargin{" + i + "}),";
            end
        end
        
%         for i=1:1:(nargin-2)    % total number of inputs - 2 (one is PV name and the other is for the last inputs)
%             if (rem(i,2)==1)
%                 str = str + "varargin{" + i + "},";
%             else
%                 str = str + "num2cell(varargin{" + i + "}),";
%             end
%         end
%         str = str + "num2cell(varargin{" + (nargin-1) + "}))";
        str = str + "num2cell(varargin{" + (num_ielements) + "}))";
        MatP4P.put(pvname, py.dict(pyargs('value', py.dict(eval(str)))));
    end
    
else    
end

% Return the new PV value for debuggin purpose
% if (string(varargin{end}) == "mpvaDebugOn")
%     fprintf('The updated PV is');
%     updated_PV = mpvaGet(pvname)
% end
if (class(varargin{end}) == "string")
    if (varargin{end} == "mpvaDebugOn")
        fprintf('The update PV is');
        updated_PV = mpvaGet(pvname)
    end
end

MatP4P.close();

end
