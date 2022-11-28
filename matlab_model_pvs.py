import numpy as np
from p4p.nt import NTTable
from p4p.server import Server as PVAServer
from p4p.server.thread import SharedPV
import time

# Define the structure of the twiss table (what are the columns called, and what data type each column has)
twiss_table_type = NTTable([("element", "s"), ("device_name", "s"),
				 ("s", "d"), ("z", "d"), ("length", "d"), ("p0c", "d"),
				 ("alpha_x", "d"), ("beta_x", "d"), ("eta_x", "d"), ("etap_x", "d"), ("psi_x", "d"),
				 ("alpha_y", "d"), ("beta_y", "d"), ("eta_y", "d"), ("etap_y", "d"), ("psi_y", "d")])

# Define the structure of the rmat table
rmat_table_type = NTTable(
			[("element", "s"), ("device_name", "s"), ("s", "d"), ("z", "d"), ("length", "d"),
			("r11", "d"), ("r12", "d"), ("r13", "d"), ("r14", "d"), ("r15", "d"), ("r16", "d"),
			("r21", "d"), ("r22", "d"), ("r23", "d"), ("r24", "d"), ("r25", "d"), ("r26", "d"),
			("r31", "d"), ("r32", "d"), ("r33", "d"), ("r34", "d"), ("r35", "d"), ("r36", "d"),
			("r41", "d"), ("r42", "d"), ("r43", "d"), ("r44", "d"), ("r45", "d"), ("r46", "d"),
			("r51", "d"), ("r52", "d"), ("r53", "d"), ("r54", "d"), ("r55", "d"), ("r56", "d"),
			("r61", "d"), ("r62", "d"), ("r63", "d"), ("r64", "d"), ("r65", "d"), ("r66", "d")])

# NOTE: p4p requires you to specify some initial value for every PV - there's not a default.
# here is where we make those default values.  This is a particularly hokey example.
twiss_table_rows = []
rmat_table_rows = []
element_name_list = ["SOL9000", "XC99", "YC99"]
device_name_list = ["SOL:IN20:111", "XCOR:IN20:112", "YCOR:IN20:113"]
for i in range(0,len(element_name_list)):
	element_name = element_name_list[i]
	device_name = device_name_list[i]
	rmat = np.eye(6)
	twiss_table_rows.append({"element": element_name, "device_name": device_name, "s": 0.0, "z": 0.0, "length": 0.0, "p0c": 6.0,
		"alpha_x": 0.0, "beta_x": 0.0, "eta_x": 0.0, "etap_x": 0.0, "psi_x": 0.0,
		"alpha_y": 0.0, "beta_y": 0.0, "eta_y": 0.0, "etap_y": 0.0, "psi_y": 0.0})
	rmat_table_rows.append({
		"element": element_name, "device_name": device_name, "s": 0.0, "z": 0.0, "length": 0.0,
		"r11": rmat[0,0], "r12": rmat[0,1], "r13": rmat[0,2], "r14": rmat[0,3], "r15": rmat[0,4], "r16": rmat[0,5],
		"r21": rmat[1,0], "r22": rmat[1,1], "r23": rmat[1,2], "r24": rmat[1,3], "r25": rmat[1,4], "r26": rmat[1,5],
		"r31": rmat[2,0], "r32": rmat[2,1], "r33": rmat[2,2], "r34": rmat[2,3], "r35": rmat[2,4], "r36": rmat[2,5],
		"r41": rmat[3,0], "r42": rmat[3,1], "r43": rmat[3,2], "r44": rmat[3,3], "r45": rmat[3,4], "r46": rmat[3,5],
		"r51": rmat[4,0], "r52": rmat[4,1], "r53": rmat[4,2], "r54": rmat[4,3], "r55": rmat[4,4], "r56": rmat[4,5],
		"r61": rmat[5,0], "r62": rmat[5,1], "r63": rmat[5,2], "r64": rmat[5,3], "r65": rmat[5,4], "r66": rmat[5,5]})

# Take the raw data and "wrap" it into the form that the PVA server needs.
# initial_twiss_table = twiss_table_type.wrap(twiss_table_rows)
# initial_rmat_table = rmat_table_type.wrap(rmat_table_rows)
initial_twiss_table = twiss_table_type.wrap(twiss_table_rows, timestamp=time.time())	# with timestamp
initial_rmat_table = rmat_table_type.wrap(rmat_table_rows, timestamp=time.time())		# with timestamp

# Define a "handler" that gets called when somebody puts a value into a PV.
# In our case, this is sort of silly, because we just blindly dump whatever
# a client sends into the PV object.
class Handler(object):
	def put(self, pv, operation):
		try:
			pv.post(operation.value(), timestamp=time.time()) # just store the value and update subscribers
			operation.done()
		except Exception as e:
			operation.done(error=str(e))

# Define the PVs that will be hosted by the server.
# CU_HXR
CU_HXR_live_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
CU_HXR_extant_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
CU_HXR_live_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())
CU_HXR_extant_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())

# CU_SXR
CU_SXR_live_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
CU_SXR_extant_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
CU_SXR_live_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())
CU_SXR_extant_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())

# SC_HXR
SC_HXR_live_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
SC_HXR_extant_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
SC_HXR_live_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())
SC_HXR_extant_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())

# SC_SXR
SC_SXR_live_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
SC_SXR_extant_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
SC_SXR_live_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())
SC_SXR_extant_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())

# SC_DIAG0
SC_DIAG0_live_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
SC_DIAG0_extant_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
SC_DIAG0_live_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())
SC_DIAG0_extant_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())

# SC_BSYD
SC_BSYD_live_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
SC_BSYD_extant_twiss_pv = SharedPV(nt=twiss_table_type, initial=initial_twiss_table, handler=Handler())
SC_BSYD_live_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())
SC_BSYD_extant_rmat_pv = SharedPV(nt=rmat_table_type, initial=initial_rmat_table, handler=Handler())

# Make the PVA Server.  This is where we define the names for each of the PVs we defined above.
# By using "PVAServer.forever", the server immediately starts running, and doesn't stop until you
# kill it.
pva_server = PVAServer.forever(providers=[{f"TBLEM:SYS0:1:CU_HXR:LIVE:TWISS": CU_HXR_live_twiss_pv,
					   f"TBLEM:SYS0:1:CU_HXR:EXTANT:TWISS": CU_HXR_extant_twiss_pv,
					   f"TBLEM:SYS0:1:CU_HXR:LIVE:RMAT": CU_HXR_live_rmat_pv,
					   f"TBLEM:SYS0:1:CU_HXR:EXTANT:RMAT": CU_HXR_extant_rmat_pv,
					   f"TBLEM:SYS0:1:CU_SXR:LIVE:TWISS": CU_SXR_live_twiss_pv,
					   f"TBLEM:SYS0:1:CU_SXR:EXTANT:TWISS": CU_SXR_extant_twiss_pv,
					   f"TBLEM:SYS0:1:CU_SXR:LIVE:RMAT": CU_SXR_live_rmat_pv,
					   f"TBLEM:SYS0:1:CU_SXR:EXTANT:RMAT": CU_SXR_extant_rmat_pv,
					   f"TBLEM:SYS0:1:SC_HXR:LIVE:TWISS": SC_HXR_live_twiss_pv,
					   f"TBLEM:SYS0:1:SC_HXR:EXTANT:TWISS": SC_HXR_extant_twiss_pv,
					   f"TBLEM:SYS0:1:SC_HXR:LIVE:RMAT": SC_HXR_live_rmat_pv,
					   f"TBLEM:SYS0:1:SC_HXR:EXTANT:RMAT": SC_HXR_extant_rmat_pv,
					   f"TBLEM:SYS0:1:SC_SXR:LIVE:TWISS": SC_SXR_live_twiss_pv,
					   f"TBLEM:SYS0:1:SC_SXR:EXTANT:TWISS": SC_SXR_extant_twiss_pv,
					   f"TBLEM:SYS0:1:SC_SXR:LIVE:RMAT": SC_SXR_live_rmat_pv,
					   f"TBLEM:SYS0:1:SC_SXR:EXTANT:RMAT": SC_SXR_extant_rmat_pv,
					   f"TBLEM:SYS0:1:SC_DIAG0:LIVE:TWISS": SC_DIAG0_live_twiss_pv,
					   f"TBLEM:SYS0:1:SC_DIAG0:EXTANT:TWISS": SC_DIAG0_extant_twiss_pv,
					   f"TBLEM:SYS0:1:SC_DIAG0:LIVE:RMAT": SC_DIAG0_live_rmat_pv,
					   f"TBLEM:SYS0:1:SC_DIAG0:EXTANT:RMAT": SC_DIAG0_extant_rmat_pv,
					   f"TBLEM:SYS0:1:SC_BSYD:LIVE:TWISS": SC_BSYD_live_twiss_pv,
					   f"TBLEM:SYS0:1:SC_BSYD:EXTANT:TWISS": SC_BSYD_extant_twiss_pv,
					   f"TBLEM:SYS0:1:SC_BSYD:LIVE:RMAT": SC_BSYD_live_rmat_pv,
					   f"TBLEM:SYS0:1:SC_BSYD:EXTANT:RMAT": SC_BSYD_extant_rmat_pv}])
