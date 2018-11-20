#!/bin/sh

# https://github.com/espressif/esp-idf/issues/2080
#There is a mismatch in the metadata length between your copy of mkspiffs and your sdkconfig:
#  SPIFFS_OBJ_META_LEN: 0
#CONFIG_SPIFFS_META_LENGTH=4
#To resolve the issue you need to do either of:
#    Download a copy of mkspiffs which is built with META_LEN=4 (https://github.com/igrr/mkspiffs/releases, look for mkspiffs-x.y.z-esp-idf archive for your platform)
#    Build mkspiffs with META_LEN=4, see https://github.com/igrr/mkspiffs#spiffs-configuration
#    Change CONFIG_SPIFFS_META_LENGTH to 0 in menuconfig. Note that you will need to disable timestamp support (CONFIG_SPIFFS_USE_MTIME) as well.

make -C ../mkspiffs CPPFLAGS="-DSPIFFS_OBJ_META_LEN=4"
