#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)
#$(shell find . -maxdepth 10 -type d)
COMPONENT_SRCDIRS := . web/ clock/ wifi/
COMPONENT_ADD_INCLUDEDIRS := .
