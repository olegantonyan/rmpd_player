MKSPIFFS := $(IDF_PATH)/../mkspiffs/mkspiffs
SPIFFS_DIR := fs
SPIFFS_TARGET := build/fs.bin
SPIFFS_ADDRESS := 0x110000
SPIFFS_SIZE := 0x100000

ESPTOOL_ALL_FLASH_ARGS += $(SPIFFS_ADDRESS) $(SPIFFS_TARGET)

fs: clean_fs
	echo "building spiffs image"
	$(MKSPIFFS) -c $(SPIFFS_DIR) -b 4096 -p 256 -s $(SPIFFS_SIZE) $(SPIFFS_TARGET)

#flash_fs: fs
#	$(IDF_PATH)/components/esptool_py/esptool/esptool.py --chip esp32 --port $(CONFIG_ESPTOOLPY_PORT) --baud $(CONFIG_ESPTOOLPY_BAUD) --before default_reset --after hard_reset write_flash -z $(SPIFFS_ADDRESS) $(SPIFFS_TARGET)

clean_fs:
	rm -f $(SPIFFS_TARGET)

# NOTE: run ./build_mkspiffs.sh first
