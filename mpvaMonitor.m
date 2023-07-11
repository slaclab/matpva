function [] = mpvaMonitor(pvname)
%
% mpvaMonitor displays the values of given EPICS PV names when it is updated.
%
%    mpvaGet(pvname)
%

% Check if the input is valid PV
try
    mpvaGet(pvname);
catch
    msg = 'Please check if pvname is valid or the PV is alive';
    error(msg)
end

string = append("python3 pva_callback.py ", pvname);
system(string)

end
