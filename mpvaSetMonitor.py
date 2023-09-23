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