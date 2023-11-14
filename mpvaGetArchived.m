function varargout = mpvaGetArchived(pvname, starttime, endtime)
%
% mpvaGetArchived returns the archived values of given EPICS PV names for the specified time range.
%
%    [NTTable, ts, alarm, NTStruct] = mpvaGetArchived(pvname, starttime, endtime)
%
%    starttime: Beginning time as a datetime object or a string in
%                   the format 'YYYY-MM-DDTHH:MM:SS.SSSZ'
%    endtime:   Ending time as a datetime object or a string in
%                   the format 'YYYY-MM-DDTHH:MM:SS.SSSZ'
%

% -----------------------------------------------------------------------------
% Title      : mpvaGetArchived
% -----------------------------------------------------------------------------
% File       : mpvaGetArchived.m
% Author     : Zach Domke, zdomke@slac.stanford.edu
% Created    : 2023-11-13
% Last update: 2023-11-13
% -----------------------------------------------------------------------------
% Description:
% Return the archived values of the given PV name for the specified time range.
%
% Note: Only works with NTTable PVs as they are the only PVAccess PVs being archived (as of 11/14/2023)
% -----------------------------------------------------------------------------
% This file is part of matpva. It is subject to the license terms in the 
% LICENSE.txt file found in the top-level directory of this distribution
% and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
% No part of matpva, including this file, may be copied, modified, 
% propagated, or distributed except according to the terms contained in 
% the LICENSE.txt file.
% -----------------------------------------------------------------------------
if nargin ~= 3
    error("Not enough input arguments. Please type an input PV, start time, and end time.")
end

% Check if the input is valid PV
if ~((class(pvname) == "char") || (class(pvname) == "string"))
    msg = 'Please check if pvname is valid or the PV is alive';
    error(msg)
end

% If starttime/endtime is a datetime, convert it to a string
if isdatetime(starttime)
    starttime.Format = 'yyyy-MM-dd''T''HH:mm:ss.SSS''Z';
    starttime = string(starttime);
end

if isdatetime(endtime)
    endtime.Format = 'yyyy-MM-dd''T''HH:mm:ss.SSS''Z';
    endtime = string(endtime);
end

% Construct the Archiver's URL
[~, accelerator] = getSystem;
accelerator = lower(accelerator);
url = ['http://' accelerator '-archapp.slac.stanford.edu/retrieval/data/getData.json'];

% Make a request to the Archiver
parameters = struct('pv', pvname, 'from', starttime, 'to', endtime);
payload = py.requests.get(url, pyargs('params', parameters));

% Fail if the request fails
varargout = {{}, {}, {}, {}};
if ~payload.ok
    varargout(1:4) = {NaN};
    return
end

json = jsondecode(string(payload.text));
data = json.data;

num_vals = numel(data);
data_type = "NTTable";  % Allows for inclusion of other data types later 

for val=1:num_vals
    TimeInSeconds = data(val).val.timeStamp.secondsPastEpoch;
    ts = datetime(TimeInSeconds, ...
        'ConvertFrom', 'epochtime', ...
        'Epoch', '1970-01-01', ...
        'Format', 'MMM dd, yyy HH:mm:ss.SSS', ...
        'TimeZone', 'UTC');
   
    alarm = data(val).val.alarm;
    
    if data_type == "NTTable"
        NTTable = struct2table(data(val).val.value);
        varargout{1}{end+1} = NTTable;
        varargout{2}{end+1} = ts;
        varargout{3}{end+1} = alarm;
        varargout{4}{end+1} = data(val).val.value;
    end
    
end