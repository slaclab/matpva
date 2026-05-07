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
CMPLR = STRICT


USR_INCLUDES += -I../../glue
# install header file
#INC =

# build share library
SHARED_LIBRARIES = YES
STATIC_BUILD = NO

USR_CPPFLAGS += -DMATLAB_APP
USR_CPPFLAGS += $(USR_CPPFLAGS_USE_MEXLOCK_$(CONFIG_MEXLOCK))
#USR_INCLUDES += -I$(MATLABDIR)/extern/include

MEXF := mpvaGetNelem
MEXF += mpvaGet
MEXF += mpvaGetStatus
MEXF += mpvaGetGraphicLimits
MEXF += mpvaGetControlLimits
MEXF += mpvaGetWarnLimits
MEXF += mpvaGetAlarmLimits
MEXF += mpvaGetPrecision
MEXF += mpvaGetUnits
MEXF += mpvaGetEnumStrings
MEXF += mpvaPut
MEXF += mpvaPutNoWait
MEXF += mpvaGetRetryCount
MEXF += mpvaSetRetryCount
MEXF += mpvaGetTimeout
MEXF += mpvaSetTimeout
MEXF += mpvaSetSeverityWarnLevel
MEXF += mpvaClear
MEXF += mpvaSetMonitor
MEXF += mpvaNewMonitorValue
MEXF += mpvaNewMonitorWait
MEXF += mpvaDelay
MEXF += mpvaLastError
MEXF += mpvaDebugOn
MEXF += mpvaDebugOff
ifeq ($(CONFIG_ECDRGET),YES)
MEXF += ecget
endif

MEXTARGET=$(addsuffix $(MEXEXT),$(MEXF))

MATDOCS=$(wildcard $(addprefix $(TOP)/documentation/help/,$(MEXF:%=%.m) Contents.m))

SCRIPTS+=$(notdir $(MATDOCS))


#       Library to build:
#       lib$(LIBRARY).a  or   ..dll/..exp/..lib
#

# need additional libs (PVAccess)
PROD_LIBS := mezcaglue pvAccess pvData Com

PROD_LIBS_WIN32+= pvAccess pvData Com

pvAccess_DIR = $(EPICS_BASE_LIB)
pvData_DIR = $(EPICS_BASE_LIB)
Com_DIR = $(EPICS_BASE_LIB)

## USR_LIBS_WIN32 += libmx libmex

## libmx_DIR     := $(MATLABDIR)/extern/lib/$(MATLIB_SUBDIR)
## libmex_DIR    := $(MATLABDIR)/extern/lib/$(MATLIB_SUBDIR)
mezcaglue_DIR := $(INSTALL_LIB)


ifndef BASE_3_14
ifeq ($(OS_CLASS),solaris)
ACC_DEP_CFLAGS = -KPIC -v
CCC_DEP_CFLAGS = -KPIC -v
endif
endif


INSTALL_PROD=$(MEXTARGET:%=$(INSTALL_BIN)/%)
# install matlab executables and help files
# to a 'mpva' subdir so 'help mpva' lists
# the 'Contents.m' summary
INSTALL_BIN:=$(INSTALL_BIN)/mpva

include $(TOP)/configure/RULES
PRODNAME=$(MEXTARGET)



$(notdir $(MATDOCS)):%.m:$(TOP)/documentation/help/%.m 
	$(CP) $^ $@

MEXLIBS = $(PROD_LDLIBS)

# Under linux (also unix or cygwin), warnings can be enabled adding
#
MEXCFLAGS_Linux=CFLAGS="\$$CFLAGS $(WARN_CFLAGS)"
#
# to the MEX command

$(PRODNAME):%$(MEXEXT):../%.c $(PROD_DEPLIBS)
	@$(RM) $@
	$(MATLABDIR)/bin/$(MEX) $(MEXCXXFLAGS) CXXFLAGS=$(MEX_CXXFLAGS) $(USR_CPPFLAGS) $(MEXCFLAGS_$(OS_CLASS)) $< $(INCLUDES) LDFLAGS="$(PRODDIR_LDFLAGS) \$$LDFLAGS" $(MEXLIBS)

build:	$(MEXTARGET)


buildInstall:
	@echo matdocs $(MATDOCS)

#	EOF Makefile.Host
