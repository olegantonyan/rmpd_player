PORT=/dev/ttyUSB0

firmware:
	docker run -e "IMAGE_NAME=firmware" --rm -ti -v $(shell pwd)/../nodemcu-firmware:/opt/nodemcu-firmware marcelstoer/nodemcu-build build

flash_firmware:
	esptool.py --port $(PORT) write_flash 0x00000 $(shell pwd)/../nodemcu-firmware/bin/nodemcu_float_firmware.bin

app_flash:
	nodemcu-uploader --port $(PORT) upload *.lua *.html --verify=raw
