function [] = mpvaMonitor(pvname)
%
% mpvaMonitor displays the values of given EPICS PV names when it is updated.
%
%    mpvaMonitor(pvname)
%

% -----------------------------------------------------------------------------
% Title      : mpvaMonitor
% -----------------------------------------------------------------------------
% File       : mpvaMonitor.m
% Author     : Kuktae Kim, ktkim@slac.stanford.edu
% Created    : 2023-11-02
% Last update: 2023-11-02
% -----------------------------------------------------------------------------
% Description:
% Display the values of given EPICS PV names when it is updated.
% -----------------------------------------------------------------------------
% This file is part of matpva. It is subject to the license terms in the 
% LICENSE.txt file found in the top-level directory of this distribution
% and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
% No part of matpva, including this file, may be copied, modified, 
% propagated, or distributed except according to the terms contained in 
% the LICENSE.txt file.
% -----------------------------------------------------------------------------
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
