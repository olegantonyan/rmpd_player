#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_SRCDIRS := . $(shell find . -maxdepth 10 -type d)
COMPONENT_ADD_INCLUDEDIRS := .
