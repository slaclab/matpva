# Code used for demo purposes from: https://mdavidsaver.github.io/p4p/server.html                                                          
# from p4p.client.thread import Context
from p4p.nt import NTScalar, NTNDArray
from p4p.server import Server, ServerOperation
from p4p.server.thread import SharedPV
import numpy as np
import time

class Handler(object):
    """ A handler for dealing with put requests to our test PVs """

    def put(self, pv: SharedPV, op: ServerOperation) -> None:
        """ Called each time a client issues a put operation on the channel using this handler """
        # pv.post(op.value())
        pv.post(op.value(),timestamp=time.time())   # Old P4P version doesn't support time.time() function
        op.done()

 
# pv = SharedPV(nt=NTScalar('d'), initial=0.0)  # scalar double  
# pv = SharedPV(nt=NTScalar('str'), initial='')  # scalar string 

# nt = NTScalar("i", display=True, control=True, valueAlarm=True)
# initial = nt.wrap({'value': 5, 'valueAlarm': {'lowAlarmLimit': 2, 'lowWarningLimit': 3, 'highAlarmLimit': 9, 'highWarningLimit': 8}}, timestamp=time.time())



initial = nt.wrap({'value': 5, }, timestamp=time.time())


int_value = SharedPV(handler=Handler(), nt=NTScalar('i'), initial=3, timestamp=time.time())

Server.forever(providers=[{'TEST:SCALAR':int_value,}]) # runs until KeyboardInterrupt
