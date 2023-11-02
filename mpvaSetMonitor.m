function ret = mpvaSetMonitor(pvname)
%
% mpvaSetMonitor subscribes to the given EPICS PV using Python class and stores values in the cache when it is updated.
% The module includes a method to check if the PV is updated since it has
% been subscribed.
%
%    PV = mpvaSetMonitor(pvname)     # Instantiate a ValueCache classs in mpvaSetMonitor Python module to monitor a pvname
%
%    mpvaNewMkonitorValue(PV)        # Return false if the pvname is not updated. Return true if it is updated.
%

% -----------------------------------------------------------------------------
% Title      : mpvaSetMonitor
% -----------------------------------------------------------------------------
% File       : mpvaSetMonitor.m
% Author     : Kuktae Kim, ktkim@slac.stanford.edu
% Created    : 2023-11-02
% Last update: 2023-11-02
% -----------------------------------------------------------------------------
% Description:
% Subscribe to the given EPICS PV using Python class and stores values in 
% the cache when it is updated.
% The module includes a method to check if the PV is updated since it has
% been subscribed.
% -----------------------------------------------------------------------------
% This file is part of matpva. It is subject to the license terms in the 
% LICENSE.txt file found in the top-level directory of this distribution
% and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
% No part of matpva, including this file, may be copied, modified, 
% propagated, or distributed except according to the terms contained in 
% the LICENSE.txt file.
% -----------------------------------------------------------------------------

% Check if the pvname is valid
try
    mpvaGet(pvname);
catch
    msg = 'Please check if pvname is valid or the PV is alive';
    error(msg)
end

% Add matpva module directory to Python search path
path = [getenv('EPICS_EXTENSIONS') '/../matpva/current'];

if count(py.sys.path,path) == 0
    insert(py.sys.path,int32(0),path);
end

MatP4P = py.p4p.client.thread.Context('pva', pyargs('nt', false));

% Return ValueCache class in mpvaSetMonitor Python module
ret = py.mpvaSetMonitor.ValueCache(MatP4P, pvname);

end
