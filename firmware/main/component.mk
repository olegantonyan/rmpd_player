#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

COMPONENT_SRCDIRS := . web/ clock/ wifi/ storage/ config/ audio/ util/ playlist/ remote/ remote/commands/ remote/commands/outgoing remote/commands/incoming system/
COMPONENT_ADD_INCLUDEDIRS := .

# certificates
COMPONENT_EMBED_TXTFILES := util/certs/letsencrypt1.pem
COMPONENT_EMBED_TXTFILES += util/certs/letsencrypt2.pem

# timezones
COMPONENT_EMBED_TXTFILES += clock/zones.json

# web files
COMPONENT_EMBED_TXTFILES += web/fs/index.html
COMPONENT_EMBED_TXTFILES += web/fs/main.js
COMPONENT_EMBED_TXTFILES += web/fs/main.css
COMPONENT_EMBED_FILES := web/fs/favicon.ico
