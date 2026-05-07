#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#	Makefile.Host

TOP = ..
include $(TOP)/configure/CONFIG

PROD_HOST += ezcaVarArrayTest

ezcaVarArrayTest_SRCS	+=	ezcaVarArrayTest.c
ezcaVarArrayTest_LIBS	+=	pvAccess pvData Com
ezcaVarArrayTest_LIBS	+=	$(EPICS_BASE_IOC_LIBS)

install: buildInstall

buildInstall: build

SCRIPTS_HOST_DEFAULT  += mpvaTest.m mpvaTest.sce
SCRIPTS_HOST_WIN32    = -nil-

USR_CFLAGS   += -I$(TOP)/glue/

#convert scilab test script to matlab
mpvaTest.m:	../mpvaTest.in
	@if ! $(SED) -e 's$$//$$%$$' -e 's/[{][{]/{/g' -e 's/[}][}]/}/g' -e 's/\<sleep(1000[*]/pause(/g' -e's/mtlb_//g' -e"s/%nan/nan('double')/g" -e 's/%[ \t]*MATLABWARN/disp/' -e 's/\([ \t]then\)\([ \t]\|$$\)/\2/g' $< > $@ ;  then \
		echo "*** WARNING: Unable to create test script for MATLAB" ;  \
		echo "%*** WARNING: Unable to create test script for MATLAB" > $@ ;  \
	fi

mpvaTest.sce: ../mpvaTest.in
	@if ! $(SED) -e 's/[{][{]/[/g' -e 's/[}][}]/]/g'  $< > $@ ;  then \
		echo "*** WARNING: Unable to create test script for SCILAB" ;  \
		echo "%*** WARNING: Unable to create test script for SCILAB" > $@ ;  \
	fi

#test:	$(TOP)/bin/$(T_A)/ezcaVarArrayTest
#	@ $*

.PHONY:	test_clean test

include $(TOP)/configure/RULES
