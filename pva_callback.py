from p4p.client.thread import Context
import time
import sys

# This python script is developed to monitor PVs using the p4p module.


def my_value_processor(value):
	"""Print updated PV.

	:param value: the PV name
	"""
	print(f'NEW: {sys.argv[1]}      {value}')


ctxt = Context('pva')
subscription = ctxt.monitor(sys.argv[1], my_value_processor)

while True:
	time.sleep(0.1)
