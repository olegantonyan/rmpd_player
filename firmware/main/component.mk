#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_SRCDIRS := . web/ clock/ wifi/ storage/ config/ audio/ util/ util/certs/ playlist/ playlist/offline playlist/cloud remote/ remote/commands/ remote/commands/outgoing remote/commands/incoming system/
COMPONENT_ADD_INCLUDEDIRS := .

# certificates
COMPONENT_EMBED_TXTFILES := util/certs/certs.pem

# timezones
COMPONENT_EMBED_TXTFILES += clock/zones.json

# web files
COMPONENT_EMBED_TXTFILES += web/fs/index.html
