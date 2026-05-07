#*************************************************************************
# Copyright (c) 2002 The University of Chicago, as Operator of Argonne
# National Laboratory.
# Copyright (c) 2002 The Regents of the University of California, as
# Operator of Los Alamos National Laboratory.
# This file is distributed subject to a Software License Agreement found
# in the file LICENSE that is included with this distribution. 
#*************************************************************************
#
# Makefile,v 1.3 2002/08/02 15:38:41 jba Exp
#
TOP = ..
T_A = cheat
include $(TOP)/configure/CONFIG

# Add HtmlTeX to path so we can use html.sty
export PATH := $(PATH):$(TOP)/documentation/HtmlTeX

# Broken: scilab 5 under windows executable is 'scilex' :-(
# but we have no T_A/OS_CLASS to test here
ifeq ($(MAKEFOR),SCILAB)
SCILAB=$(SCILABDIR)/bin/scilab
endif

# Check for tools
ifndef NO_DOCS
FOUND_PDFLATEX:=$(shell (pdflatex -v > /dev/null 2>&1 && echo YES) || echo NO)
FOUND_LATEX2HTML:=$(shell (latex2html -v > /dev/null 2>&1 && echo YES) || echo NO)
FOUND_LYNX:=$(shell (lynx -version > /dev/null 2>&1 && echo YES) || echo NO)
else
FOUND_PDFLATEX:=DONTGEN
FOUND_LATEX2HTML:=DONTGEN
FOUND_LYNX:=DONTGEN
endif

HAVE_PDFLATEX:=$(FOUND_PDFLATEX)
HAVE_LATEX2HTML:=$(FOUND_LATEX2HTML)
HAVE_LYNX:=$(FOUND_LYNX)

FOUND_PDFLATEX_MSG_NO="WARNING: 'pdflatex' tool not found; no documentation of help files will be created"
FOUND_LATEX2HTML_MSG_NO="WARNING: 'latex2html' tool not found; will skip creating HTML documentation and on-line help files"
FOUND_LYNX_MSG_NO="WARNING: 'lynx' tool not found; will skip creating on-line help files"

ifeq ($(FOUND_LYNX),NO)
$(warning $(FOUND_PDFLATEX_MSG_NO))
endif
ifeq ($(FOUND_LATEX2HTML),NO)
$(warning $(FOUND_LATEX2HTML_MSG_NO))
HAVE_LYNX:=NO
endif
ifeq ($(FOUND_PDFLATEX),NO)
$(warning $(FOUND_LYNX_MSG_NO))
HAVE_LYNX:=NO
HAVE_LATEX2HTML:=NO
endif

TEST_D=perl -MExtUtils::Command -e test_d

DIFF_F=perl -MFile::Compare -e compare

HELP_AUTOGEN_CHAPTERS:=$(shell $(SED) -n -e 's/\(.*[^b]subsection.*[{][ \t]*\)\(mpva[a-zA-Z0-9]*\).*/\2/gp' manual.tex)

HELPS=$(patsubst %,help/%.html,$(HELP_AUTOGEN_CHAPTERS))

MHELPS=$(HELPS:%.html=%.m)

# Obtain the functions which share help documentation so we can generate .m files for both the getter and setter
HELPS_SHARED_GET:=$(shell $(SED) -n -e 's/\(.*[^b]subsection.*[{][ \t]*\)\(mpva[a-zA-Z0-9]*\)\(,\smpva[a-zA-Z0-9]*\).*/\2/gp' manual.tex)

HELPS_SHARED_SET:=$(shell $(SED) -n -e 's/\(.*[^b]subsection.*[{][ \t]*\)\(mpva[a-zA-Z0-9]*\)\(,\s\)\(mpva[a-zA-Z0-9]*\).*/\4/gp' manual.tex)

MHELPS_COPY_FROM=$(patsubst %,help/%.m,$(HELPS_SHARED_GET))

MHELPS_NEEDING_COPY=$(patsubst %,help/%.m,$(HELPS_SHARED_SET))

ifneq '$(wildcard $(SCILAB))' ''
SCI5HELPS=$(patsubst %,xmlhelp/en_US/%.xml,$(HELP_AUTOGEN_CHAPTERS))
SCI5HELPS+=xmlhelp/en_US/Common.xml
SCI5HELPS+=xmlhelp/en_US/Error.xml
endif

DOCS_YES=manual.pdf
# install if found
DOCS_DONTGEN=$(wildcard $(DOCS_YES))
MANHTMLS_YES=$(wildcard $(addprefix manual/*,.html .png .css .svg))
HTMLS_YES=$(HELPS) help/whatis.htm $(MANHTMLS)

DOCS+=$(DOCS_$(HAVE_PDFLATEX))
MANHTMLS=$(MANHTMLS_$(HAVE_LATEX2HTML))
HTMLS+=$(HTMLS_$(HAVE_LYNX))

GH_PAGES_DOCS=$(DOCS) $(MANHTMLS)

INSTALL_SCI5_HELPJAR_YES=$(INSTALL_LOCATION)/jar/scilab_en_US_help.jar
# install if found
INSTALL_SCI5_HELPJAR_DONTGEN=$(addprefix $(INSTALL_LOCATION)/,$(wildcard jar/scilab_en_US_help.jar))
INSTALL_SCI5_HELPJAR=$(INSTALL_SCI5_HELPJAR_$(HAVE_LYNX))

$(INSTALL_HTML)/$(HTMLS_DIR)/%: %
	@echo "Installing HTML Help/Documentation $@"
	@$(INSTALL) -d -m 644 $< $(@D)

$(INSTALL_DOC)/%:%
	@echo "Installing PDF Documentation $@"
	@$(INSTALL) -d -m 644 $< $(@D)

INSTALL_GH_PAGES=$(addprefix $(TOP)/gh-pages/,$(GH_PAGES_DOCS))

$(TOP)/gh-pages/%: %
	@($(TEST_D) $(@D) && \
		echo "Installing github Help/Documentation $@" && \
		$(INSTALL) -d -m 644 $< $(@D) ) || \
		echo "No $(TOP)/gh-pages/ directory; skipping installation" 

NULLSTRING:=
SP:=$(NULLSTRING) #

install: buildInstall
	@$(subst @,$(SP),$(addprefix echo ,$(subst $(SP),@,$(FOUND_PDFLATEX_MSG_$(FOUND_PDFLATEX)))))
	@$(subst @,$(SP),$(addprefix echo ,$(subst $(SP),@,$(FOUND_LATEX2HTML_MSG_$(FOUND_LATEX2HTML)))))
	@$(subst @,$(SP),$(addprefix echo ,$(subst $(SP),@,$(FOUND_LYNX_MSG_$(FOUND_LYNX)))))

buildInstall: build $(INSTALL_DOCS) $(INSTALL_HTMLS) $(INSTALL_GH_PAGES)

ifneq '$(wildcard $(SCILAB))' ''
buildInstall: $(INSTALL_SCI5_HELPJAR)
endif
clean::
	$(RM) gitstring.tex gitstring.tex.new

$(sort manual/index.html $(wildcard manual/*.html)): manual.aux
	latex2html -no_footnode -split 5 -link 1 manual.tex

$(HELPS): manual.aux
	$(TEST_D) help || $(MKDIR) help
	latex2html -no_footnode -split 5 -link 0 -no_navigation -no_antialias_text -long_titles 1 -dir help manual.tex

gitstring.tex: .FORCE
	@$(RM) $@.new
	@echo '%newcommand{%gitstring}{%lstinline|$(GITSTRING)|}' | tr '%' '\\' > $@.new
	@$(PERLDIFF) $@.new $@ || ($(RM) $@; perl -e 'rename("$@.new","$@")')
	@$(RM) $@.new

ifndef NO_DOCS
manual.pdf manual.aux: manual.tex gitstring.tex
	pdflatex $<
	pdflatex $<
endif


help/whatis.htm: whatis.htm
	$(TEST_D) help || $(MKDIR) help
	$(RM) $@
	cp $< $@

help/Contents.m: Contents.m
	$(TEST_D) help || $(MKDIR) help
	$(RM) $@
	cp $< $@

$(MHELPS_NEEDING_COPY): $(MHELPS)
	$(subst ^, ,$(join $(addprefix cp^,$(MHELPS_COPY_FROM)),$(patsubst %,^%;,$(MHELPS_NEEDING_COPY))))

# html->text, add leading '%' and remove long URLs, just leave filename
%.m:%.html
	$(RM) $@
	lynx -dump $^ | $(SED) -e 's/^/%/g'  -e 's/\([0-9]*[.][ ]*\)\(file:\([^/]*[/]\)*\)\(.*\)\([.]html.*\)/\4 \1\4/g' > $@

ifeq ($(HAVE_LATEX2HTML),YES)
build: manual/index.html
else
build:
endif

ifeq ($(HAVE_LYNX),YES)
build: $(MHELPS) help/Contents.m
build: $(MHELPS_NEEDING_COPY)
endif

# make again for the HTMLS wildcard expansion to find the newly created files
ifeq ($(MANHTMLS)xx$(HAVE_LATEX2HTML),xxYES)
build:
	$(MAKE)
endif

xmlclean:
	$(RM)    xmlhelp/en_US/mpva*.xml
	$(RM)    xmlhelp/en_US/mpva*.sci
	$(RM)    xmlhelp/en_US/master_help.xml
	$(RM) -r xmlhelp/en_US/scilab_en_US_help/
	$(RM) -r jar

# prepare '.sci' file based on '.m'. This must be hand-tuned
# to a xmlhelp/en_US/%.sci file which can be translated into xml by
# help_from_sci()
xmlhelp/en_US/%.sci: help/%.m
	$(SED) -e '1s/^%/function /' -e '1,$$s#^%#//#' -e 's/References/See also/' -e '$$a\
endfunction' $^ > $@	
	# extract first sentence and insert into first line
	# insert as second line
	$(SED) $@ -i -e "1a\// `$(SED) $@ -e 's#^[/][/]##g' | tr '\n' ' ' | $(SED) -e 's/\(.*Description[ \t]*\)\([^.]*[.]\)\(.*\)/\2/'`"


xmlhelp/en_US/mpva%.xml: xmlhelp/en_US/mpva%.sci
	$(RM) $@
	$(SCILAB) -nb -nw -e 'help_from_sci("$(@:%.xml=%.sci)","./xmlhelp/en_US"); quit;'

ifndef NO_DOCS
./jar/scilab_en_US_help.jar: $(SCI5HELPS)
	$(RM) $@
	$(SCILAB) -nb -nw -e 'xmltojar("xmlhelp/en_US","MPVA"); quit'
endif

$(INSTALL_LOCATION)/jar/scilab_en_US_help.jar: ./jar/scilab_en_US_help.jar $(INSTALL_LOCATION)/jar 
	@echo "Installing SCILAB Help $@"
	@$(INSTALL) -d -m 644 $< $(@D)

$(INSTALL_LOCATION)/jar:
	-$(TEST_D) $@ || $(MKDIR) $@
	

.SECONDARY:

realclean:: clean xmlclean
	$(RM) -r help
	$(RM) -r manual 
	$(RM) *.aux *.dvi *.log *.pdf *.out *.ps
	$(RM) *~

.PHONY: .FORCE

test:
	echo $(SCI5HELPS)
