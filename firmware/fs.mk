# NOTE: run ./build_mkspiffs.sh first

MKSPIFFS := $(IDF_PATH)/../mkspiffs/mkspiffs
SPIFFS_DIR := fs
SPIFFS_TARGET := build/fs.bin
SPIFFS_ADDRESS := 0x190000
SPIFFS_SIZE := 0x80000

ESPTOOL_ALL_FLASH_ARGS += $(SPIFFS_ADDRESS) $(SPIFFS_TARGET)

fs: clean_fs
	echo "building spiffs image"
	$(MKSPIFFS) -c $(SPIFFS_DIR) -b 4096 -p 256 -s $(SPIFFS_SIZE) $(SPIFFS_TARGET)

clean_fs:
	rm -f $(SPIFFS_TARGET)
