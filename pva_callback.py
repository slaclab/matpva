from p4p.client.thread import Context
import time
import sys


def my_value_processor(value):
	""" Print updated PV. """
	print(f'NEW: {sys.argv[1]}      {value}')


ctxt = Context('pva')
subscription = ctxt.monitor(sys.argv[1], my_value_processor)

while True:
	time.sleep(0.1)
