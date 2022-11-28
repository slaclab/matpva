from p4p.nt import NTTable, NTScalar
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

table_type = NTTable([("name", "s"), ("number", "d")])
table_rows = []
table_rows.append({"name": "device_1", "number": 1.1})
table_rows.append({"name": "device_2", "number": 1.2})
# initial_table = table_type.wrap(table_rows)
initial_table = table_type.wrap(table_rows, timestamp=time.time())    # Old P4P version doesn't support time.time() function
table_pv = SharedPV(nt=table_type, initial=initial_table, handler=Handler())
server = Server.forever(providers=[{"TEST:TBL": table_pv}])


# table_type = NTTable([("element", "s"), ("device_name", "s"), ("s", "i"), ("z", "i"),  ])
# table_rows = []
# table_rows.append({"name": "device_1", "number": 1.1})
# table_rows.append({"name": "device_2", "number": 1.2})
# initial_table = table_type.wrap(table_rows, timestamp=time.time())
# table_pv = SharedPV(nt=table_type, initial=initial_table, handler=Handler())
# server = Server.forever(providers=[{"KTKIM:SYS0:1:CU_HXR:EXTANT:TWISS": table_pv}])

