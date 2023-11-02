# -----------------------------------------------------------------------------
# Title      : mpvaSetMonitor
# -----------------------------------------------------------------------------
# File       : mpvaSetMonitor.py
# Author     : Kuktae Kim, ktkim@slac.stanford.edu
# Created    : 2023-11-02
# Last update: 2023-11-02
# -----------------------------------------------------------------------------
# Description:
# Python class for mpvaSetMonitor function.
# -----------------------------------------------------------------------------
# This file is part of matpva. It is subject to the license terms in the 
# LICENSE.txt file found in the top-level directory of this distribution
# and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
# No part of matpva, including this file, may be copied, modified, 
# propagated, or distributed except according to the terms contained in 
# the LICENSE.txt file.
# -----------------------------------------------------------------------------

# mpvaSetMonitor.py
from p4p.client.thread import Context, Disconnected

class ValueCache:
    def __init__(self, ctxt: Context, pv: str):
        self.__cache = None
        self.updated = False
        self.__sub = ctxt.monitor(pv, self.__update, notify_disconnect=True)
        
    def __update(self, V): # Value or Exception
        if isinstance(V, Disconnected):
            self.__cache = None
            self.updated = False
        elif self.__cache is None:
            self.__cache = V
            self.updated = False
        else:
            self.__cache[None] = V
            self.updated = True

    def mpvaNewMonitorValue(self):
        return self.updated

    @property
    def current(self):
        return self.__cache