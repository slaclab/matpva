from p4p.client.thread import Context
import time
import sys
from tabulate import tabulate

# This python script is developed to monitor PVs using the p4p module.


def my_value_processor(pv):
	"""Print updated PV.

	:param value: the PV name
	"""
	#print(f'NEW: {sys.argv[1]}      {value}')
	if "NTTable" in pv.getID():
		print(f'NEW: {sys.argv[1]}   {time.asctime(time.localtime(pv.timeStamp.secondsPastEpoch))}')
		print(tabulate(pv.value.todict(), headers="keys"))
	else:
		print(f'NEW: {sys.argv[1]}   {time.asctime(time.localtime(pv.timeStamp.secondsPastEpoch))}   {pv.value}')


ctxt = Context('pva', nt=False)
subscription = ctxt.monitor(sys.argv[1], my_value_processor)

while True:
	time.sleep(0.1)
