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
%path = getenv('EPICS_EXTENSIONS') + "/../matpva/";
%string = append("python", path, "pva_callback.py", pvname);
string = append("python pva_callback.py ", pvname);
system(string)

end
