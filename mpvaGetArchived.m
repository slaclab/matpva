function varargout = mpvaGetArchived(pvname, starttime, endtime)
%
% mpvaGetArchived returns the archived values of given EPICS PV names for the specified time range.
%
%    [NTTable, ts, alarm, NTStruct] = mpvaGetArchived(pvname, starttime, endtime)
%
%    starttime: Beginning time as a datetime object or a string in
%                   format 'mm/dd/yyyy hh:mm:ss'
%    endtime:   Ending time as a datetime object or a string in
%                   format 'mm/dd/yyyy hh:mm:ss'
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

% Convert timestamps to strings in ISO 8601 format in UTC time
[start_str, end_str] = local2utc(starttime, endtime);

% Get the descriptor of the current accelerator. Used in constructing the Archiver's URL
[~, accelerator] = getSystem;

% Construct the Archiver's URL
accelerator = lower(accelerator);
url = ['http://' accelerator '-archapp.slac.stanford.edu/retrieval/data/getData.json'];

% Make a request to the Archiver
parameters = struct('pv', pvname, 'from', start_str, 'to', end_str);
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
    NanoSeconds = data(val).val.timeStamp.nanoseconds * 10^-9;
    ts = datetime(TimeInSeconds + NanoSeconds, ...
        'ConvertFrom', 'epochtime', ...
        'Epoch', '1970-01-01', ...
        'Format', 'MMM dd, yyy HH:mm:ss.SSS', ...
        'TimeZone', 'UTC');
    ts.TimeZone = 'America/Los_Angeles';
   
    alarm = data(val).val.alarm;
    
    if data_type == "NTTable"
        NTTable = struct2table(data(val).val.value);
        varargout{1}{end+1} = NTTable;
        varargout{2}{end+1} = ts;
        varargout{3}{end+1} = alarm;
        varargout{4}{end+1} = data(val).val.value;
    end
    
end
end


function [start_str, end_str] = local2utc(starttime, endtime)
    % Return timestamp as a string in UTC in ISO 8601 format
    infmt = 'MM/dd/yyyy HH:mm:ss';

    starttime = datetime(starttime, 'InputFormat', infmt, 'TimeZone', 'America/Los_Angeles');
    starttime.TimeZone = 'UTC';
    starttime.Format = 'yyyy-MM-dd''T''HH:mm:ss.SSS''Z';
    start_str = string(starttime);
    
    endtime = datetime(endtime, 'InputFormat', infmt, 'TimeZone', 'America/Los_Angeles');
    endtime.TimeZone = 'UTC';
    endtime.Format = 'yyyy-MM-dd''T''HH:mm:ss.SSS''Z';
    end_str = string(endtime);
end
