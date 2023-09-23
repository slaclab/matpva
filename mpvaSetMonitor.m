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

% Check if the pvname is valid
try
    mpvaGet(pvname);
catch
    msg = 'Please check if pvname is valid or the PV is alive';
    error(msg)
end

% Add matpva module directory to Python search path
path = [getenv('EPICS_EXTENSIONS') '/../matpva'];

if count(py.sys.path,path) == 0
    insert(py.sys.path,int32(0),path);
end


MatP4P = py.p4p.client.thread.Context('pva', pyargs('nt', false));

% Return ValueCache class in mpvaSetMonitor Python module
ret = py.mpvaSetMonitor.ValueCache(MatP4P, pvname);

end
