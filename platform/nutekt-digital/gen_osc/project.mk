# #############################################################################
# Project Customization
# #############################################################################

PROJECT = gen_osc

UCSRC = 

UCXXSRC = $(PROJECTDIR)/gen_exported/gen_dsp/genlib.cpp\
					$(PROJECTDIR)/gen_exported/gen_exported.cpp\
					gen_osc.cpp

UINCDIR = $(PROJECTDIR)/gen_exported/gen_dsp\
					$(PROJECTDIR)/gen_exported

UDEFS =

ULIB = 

ULIBDIR =
