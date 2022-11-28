MatP4P = py.p4p.client.thread.Context('pva')

% a=MatP4P.get('KTKIM:TBL')

% aa=py.scipy.io.savemat('export.mat',a)

b=MatP4P.get('SIOC:B15:RF03:0:STR0:STREAM_SLOWSHORT0')

% bb=py.scipy.io.savemat('export.mat',b)