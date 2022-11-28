# Code used for demo purposes from: https://mdavidsaver.github.io/p4p/server.html                                                          
from p4p.client.thread import Context
from p4p.nt import NTScalar, NTNDArray
from p4p.server import Server, ServerOperation
from p4p.server.thread import SharedPV
import numpy as np
import time

# from p4p.client.thread import Context
# from p4p.nt import NTNDArray, NTScalar
# from p4p.server import Server, ServerOperation
# from p4p.server.thread import SharedPV
# import numpy as np
# import random
# import threading
 
# pv = SharedPV(nt=NTScalar('d'), initial=0.0)  # scalar double  
pv = SharedPV(nt=NTScalar('str'), initial='')  # scalar string 
 
@pv.put
def handle(pv, op):
	# pv.post(op.value()) # just store and update subscribers  
	pv.post(op.value(),timestamp=time.time())   # Old P4P version doesn't support time.time() function                                                                              
	op.done()
			    
Server.forever(providers=[{'TEST:SCALAR':pv,}]) # runs until KeyboardInterrupt
