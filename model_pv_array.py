# Code used for demo purposes from: https://mdavidsaver.github.io/p4p/server.html                                                          
from p4p.nt import NTScalar, NTNDArray
from p4p.server import Server, ServerOperation
from p4p.server.thread import SharedPV
import numpy as np
import time

 
# @pv.put
# def handle(pv, op):
# 	# pv.post(op.value()) # just store and update subscribers   
# 	pv.post(op.value(),timestamp=time.time())   # Old P4P version doesn't support time.time() function                                                                             
# 	op.done()

class Handler(object):
    """ A handler for dealing with put requests to our test PVs """

    def put(self, pv: SharedPV, op: ServerOperation) -> None:
        """ Called each time a client issues a put operation on the channel using this handler """
        pv.post(op.value(), timestamp=time.time())  # Add timestamp
        op.done()

pv1 = SharedPV(handler=Handler(), nt=NTScalar('ad'), initial=np.zeros(3))  # scalar array double    
pv2 = SharedPV(handler=Handler(), nt=NTScalar('ai'), initial=np.zeros(3, dtype=np.int32))  # scalar array integer
pv3 = SharedPV(handler=Handler(), nt=NTScalar('as'), initial='')  # scalar array string      


Server.forever(providers=[{'TEST:ARRAYd':pv1, 'TEST:ARRAYi':pv2, 'TEST:ARRAYs':pv3}]) # runs until KeyboardInterrupt
