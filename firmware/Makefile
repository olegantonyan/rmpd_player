#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

IDF_PATH := $(shell pwd)/../esp-idf
export IDF_PATH

PROJECT_NAME := firmware

include $(IDF_PATH)/make/project.mk


MKSPIFFS := $(IDF_PATH)/../mkspiffs/mkspiffs
SPIFFS_DIR := fs
SPIFFS_TARGET := build/fs.bin
SPIFFS_ADDRESS := 0x110000

fs: clean_fs
	$(MKSPIFFS) -c $(SPIFFS_DIR) -b 4096 -p 256 -s 0x100000 $(SPIFFS_TARGET)

flash_fs: fs
	$(IDF_PATH)/components/esptool_py/esptool/esptool.py --chip esp32 --port $(CONFIG_ESPTOOLPY_PORT) --baud $(CONFIG_ESPTOOLPY_BAUD) --before default_reset --after hard_reset write_flash -z $(SPIFFS_ADDRESS) $(SPIFFS_TARGET)

clean_fs:
	rm -f $(SPIFFS_TARGET)
