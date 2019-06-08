#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

IDF_PATH := $(shell pwd)/../esp-idf
export IDF_PATH

PROJECT_NAME := firmware
PROJECT_VER := "1.0.0"

include $(IDF_PATH)/make/project.mk

webapp:
	$(shell cd webapp && npm run-script build > /dev/null && npm run-script deploy > /dev/null)

.PHONY: webapp
