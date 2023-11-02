function [] = mpvaMonitor(pvname)
%
% mpvaMonitor displays the values of given EPICS PV names when it is updated.
%
%    mpvaMonitor(pvname)
%

% Check if the input is valid PV
try
    mpvaGet(pvname);
catch
    msg = 'Please check if pvname is valid or the PV is alive';
    error(msg)
end

% Bring the Python script for the mpvpaMonitor
string = append("python3 pva_callback.py ", pvname);
system(string)

end
