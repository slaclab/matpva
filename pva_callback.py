from p4p.client.thread import Context
import time
 
def my_value_processor(value):
	""" Adds three to the value and prints it. """
	print(f'After adding 3, the PV has a value of: {value+3}')
						 
ctxt = Context('pva')
subscription = ctxt.monitor('your_username:pva_temp', my_value_processor)

while True:
	time.sleep(0.1)
