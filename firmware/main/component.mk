#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_SRCDIRS := . web/ clock/ wifi/ storage/ config/ audio/ util/ playlist/ remote/ remote/commands/ remote/commands/outgoing
COMPONENT_ADD_INCLUDEDIRS := .

COMPONENT_EMBED_TXTFILES := util/certs/letsencrypt1.pem
COMPONENT_EMBED_TXTFILES += util/certs/letsencrypt2.pem

COMPONENT_EMBED_TXTFILES += clock/zones.json
