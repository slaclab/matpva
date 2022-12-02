from p4p.client.thread import Context
from p4p.nt import NTNDArray, NTScalar, NTTable
from p4p.server import Server, ServerOperation
from p4p.server.thread import SharedPV
import numpy as np
import random
import threading
import time


class Handler(object):
    """ A handler for dealing with put requests to our test PVs """

    def put(self, pv: SharedPV, op: ServerOperation) -> None:
        """ Called each time a client issues a put operation on the channel using this handler """
        pv.post(op.value(), timestamp=time.time())  # Add timestamp
        op.done()


class PVServer(object):
    """ A simple server created by p4p for making some PVs to read and write test data to """
    def __init__(self):
        self.create_pvs()
        self.context = Context('pva', nt=False)  # Disable automatic value unwrapping
        self.event = threading.Event()
        print('Creating testing server...')
        self.server_thread = threading.Thread(target=self.run_server)
        self.server_thread.start()
        # Do not allow to update for now
        # self.update_thread = threading.Thread(target=self.update_pvs, daemon=True)
        # self.update_thread.start()

    def create_pvs(self) -> None:
        """ Create a few PVs for interacting with """
        handler = Handler()  # A simple default handler that will just post PV updates is sufficient here
     
        # A scalar int value with some alarm limits set
        nt = NTScalar("i", display=True, control=True, valueAlarm=True)
        initial = nt.wrap({'value': 5, 'valueAlarm': {'lowAlarmLimit': 2, 'lowWarningLimit': 3,
                                                      'highAlarmLimit': 9, 'highWarningLimit': 8}})
        self.int_value = SharedPV(handler=handler, nt=NTScalar('i'), initial=initial)
        self.int_value2 = SharedPV(handler=handler, nt=NTScalar('i'), initial=3)
        self.uint_value = SharedPV(handler=handler, nt=NTScalar('I'), initial=1333333333)
        self.byte_value = SharedPV(handler=handler, nt=NTScalar('b'), initial=12)
        self.ubyte_value = SharedPV(handler=handler, nt=NTScalar('B'), initial=77)
        self.short_value = SharedPV(handler=handler, nt=NTScalar('h'), initial=1)
        self.ushort_value = SharedPV(handler=handler, nt=NTScalar('H'), initial=10000)
        self.long_value = SharedPV(handler=handler, nt=NTScalar('l'), initial=12345)
        self.ulong_value = SharedPV(handler=handler, nt=NTScalar('L'), initial=1234567890123456789)
        self.float_value = SharedPV(handler=handler, nt=NTScalar('f'), initial=0.0)
        self.double_value = SharedPV(handler=handler, nt=NTScalar('d'), initial=0.0)
        self.bool_value = SharedPV(handler=handler, nt=NTScalar('?'), initial=False)
        self.string_value = SharedPV(handler=handler, nt=NTScalar('s'), initial='Test!')

        # An array values
        self.int_array = SharedPV(handler=handler, nt=NTScalar('ai'), initial=[1, 2, 3, 4, 5])
        self.uint_array = SharedPV(handler=handler, nt=NTScalar('aI'), initial=[5, 4, 3, 2, 1])
        self.byte_array = SharedPV(handler=handler, nt=NTScalar('ab'), initial=[0, 1, 1, 0, 0])
        self.ubyte_array = SharedPV(handler=handler, nt=NTScalar('aB'), initial=[0, 1, 1, 0, 0])
        self.short_array = SharedPV(handler=handler, nt=NTScalar('ah'), initial=[0, 1, 1, 0, 0, 0, 1, 1])
        self.ushort_array = SharedPV(handler=handler, nt=NTScalar('aH'), initial=[0, 123, 123, 0, 0, 0, 123, 123])
        self.long_array = SharedPV(handler=handler, nt=NTScalar('al'), initial=[123, 234, 345])
        self.ulong_array = SharedPV(handler=handler, nt=NTScalar('aL'), initial=[123456, 234567, 345678])
        self.float_array = SharedPV(handler=handler, nt=NTScalar('af'), initial=[1.5, 2.5, 3.5, 4.5, 5.5])
        self.double_array = SharedPV(handler=handler, nt=NTScalar('ad'), initial=[1.5, 2.5, 3.5, 4.5, 5.5])
        self.wave_form = SharedPV(handler=handler, nt=NTScalar('af'), initial=[1.0, 2.0, 3.0, 4.0])
        self.bool_array = SharedPV(handler=handler, nt=NTScalar('a?'), initial=[True, False, True, False])
        self.string_array = SharedPV(handler=handler, nt=NTScalar('as'), initial=['One', 'Two', 'Three'])

        # An NTNDArray that will be used to hold image data
        self.image_pv = SharedPV(handler=handler, nt=NTNDArray(), initial=np.zeros(1))

        # A NTTable
        table_type = NTTable([("name", "s"), ("number", "d")])
        table_rows = []
        table_rows.append({"name": "device_1", "number": 1.1})
        table_rows.append({"name": "device_2", "number": 1.2})
        initial_table = table_type.wrap(table_rows, timestamp=time.time())    # Old P4P version doesn't support time.time() function
        self.nt_table = SharedPV(handler=handler, nt=table_type, initial=initial_table)

    def run_server(self) -> None:
        """ Run the server that will provide the PVs until keyboard interrupt """
        Server.forever(providers=[{'TEST:PVA:IntValue': self.int_value,
                                   'TEST:PVA:IntValue2': self.int_value2,
                                   'TEST:PVA:uIntValue': self.uint_value,
                                   'TEST:PVA:ByteValue': self.byte_value,
                                   'TEST:PVA:uByteValue': self.ubyte_value,
                                   'TEST:PVA:ShortValue': self.short_value,
                                   'TEST:PVA:uShortValue': self.ushort_value,
                                   'TEST:PVA:LongValue': self.long_value,
                                   'TEST:PVA:uLongValue': self.ulong_value,
                                   'TEST:PVA:FloatValue': self.float_value,
								   'TEST:PVA:DoubleValue': self.double_value,
                                   'TEST:PVA:BoolValue': self.bool_value,                                   
                                   'TEST:PVA:StringValue': self.string_value,
                                   'TEST:PVA:IntArray': self.int_array,
                                   'TEST:PVA:uIntArray': self.uint_array,
                                   'TEST:PVA:ShortArray': self.short_array,
                                   'TEST:PVA:uShortArray': self.ushort_array,
                                   'TEST:PVA:ByteArray': self.byte_array,
                                   'TEST:PVA:uByteArray': self.ubyte_array,
                                   'TEST:PVA:LongArray': self.long_array,
                                   'TEST:PVA:uLongArray': self.ulong_array,
                                   'TEST:PVA:FloatArray': self.float_array,
								   'TEST:PVA:DoubleArray': self.double_array,
                                   'TEST:PVA:Waveform': self.wave_form,
                                   'TEST:PVA:BoolArray': self.bool_array,
                                   'TEST:PVA:StringArray': self.string_array,
                                   'TEST:PVA:Image': self.image_pv,
                                   'TEST:PVA:NTTable': self.nt_table}])

    def gaussian_2d(self, x: float, y: float, x0: float, y0: float, xsig: float, ysig: float) -> np.ndarray:
        return np.exp(-0.5 * (((x - x0) / xsig) ** 2 + ((y - y0) / ysig) ** 2))

    def update_pvs(self) -> None:
        """ Continually update the value of some PVs """
        while True:
            self.event.wait(0.7)
            x = np.linspace(-5.0, 5.0, 512)
            y = np.linspace(-5.0, 5.0, 512)
            x0 = 0.5 * (np.random.rand() - 0.5)
            y0 = 0.5 * (np.random.rand() - 0.5)
            xsig = 0.8 - 0.2 * np.random.rand()
            ysig = 0.8 - 0.2 * np.random.rand()
            xgrid, ygrid = np.meshgrid(x, y)
            z = self.gaussian_2d(xgrid, ygrid, x0, y0, xsig, ysig)
            image_data = np.abs(256.0 * (z)).flatten(order='C').astype(np.uint8, copy=False)
            self.context.put('TEST:PVA:Image',
                             {'value': image_data,
                              'dimension': [{'size': 512, 'offset': 0}, {'size': 512, 'offset': 0}]
                              })
            self.context.put('TEST:PVA:IntValue', random.randint(1, 100))
            self.context.put('TEST:PVA:FloatValue', random.uniform(1, 100))
            self.context.put('TEST:PVA:BoolValue', random.choice([True, False]))
            self.context.put('TEST:PVA:FloatArray', (np.random.rand(5) * 10).tolist())


if __name__ == '__main__':
    try:
        print('Starting PVA testing ioc...')
        server = PVServer()
    except KeyboardInterrupt:
        print('\nInterrupted... finishing PVA testing ioc')
