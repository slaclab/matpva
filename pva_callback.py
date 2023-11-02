# -----------------------------------------------------------------------------
# Title      : pva callback
# -----------------------------------------------------------------------------
# File       : pva_callback.py
# Author     : Kuktae Kim, ktkim@slac.stanford.edu
# Created    : 2023-11-02
# Last update: 2023-11-02
# -----------------------------------------------------------------------------
# Description:
# Python script for mpvaMonitor function.
# -----------------------------------------------------------------------------
# This file is part of matpva. It is subject to the license terms in the 
# LICENSE.txt file found in the top-level directory of this distribution
# and at: https://confluence.slac.stanford.edu/display/ppareg/LICENSE.html. 
# No part of matpva, including this file, may be copied, modified, 
# propagated, or distributed except according to the terms contained in 
# the LICENSE.txt file.
# -----------------------------------------------------------------------------
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
