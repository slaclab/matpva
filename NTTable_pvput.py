from p4p.client.thread import Context  # Bring P4P module
import time

MatP4P = Context('pva')	 # Set MatP4P as a p4p.client.thread.Context object
MatP4P.get('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS')

# # New values for the PV
# MatP4P.put('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS',{'value.element':['SOL9000', 'XC99', 'YC99'],
# 'value.device_name':['SOL:IN20:111', 'XCOR:IN20:112', 'YCOR:IN20:113'], 'value.s':[1,2,3], 'value.z':[10, 11, 12],
# 'value.length':[100, 200, 300], 'value.p0c':[6, 16, 26], 'value.alpha_x':[0.1, 0.2, 0.3],
# 'value.beta_x':[0.1, 0.2, 0.3], 'value.eta_x':[0.1, 0.2, 0.3], 'value.etap_x':[0.1, 0.2, 0.3], 'value.psi_x':[0.1, 0.2, 0.3],
# 'value.alpha_y':[0.1, 0.2, 0.3], 'value.beta_y':[0.1, 0.2, 0.3], 'value.eta_y':[0.1, 0.2, 0.3], 'value.etap_y':[0.1, 0.2, 0.3], 'value.psi_y':[0.1, 0.2, 0.3]})

# Initial values for the PV
MatP4P.put('TBLEM:SYS0:1:CU_HXR:LIVE:TWISS',{'value.element':['SOL9000', 'XC99', 'YC99'],
'value.device_name':['SOL:IN20:111', 'XCOR:IN20:112', 'YCOR:IN20:113'], 'value.s':[0,0,0], 'value.z':[0,0,0],
'value.length':[0,0,0], 'value.p0c':[6, 6, 6], 'value.alpha_x':[0,0,0],
'value.beta_x':[0,0,0], 'value.eta_x':[0,0,0], 'value.etap_x':[0,0,0], 'value.psi_x':[0,0,0],
'value.alpha_y':[0,0,0], 'value.beta_y':[0,0,0], 'value.eta_y':[0,0,0], 'value.etap_y':[0,0,0], 'value.psi_y':[0,0,0]})