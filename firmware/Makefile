#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

IDF_PATH := $(shell pwd)/../esp-idf
export IDF_PATH

PROJECT_NAME := firmware

include $(IDF_PATH)/make/project.mk

include fs.mk

flash_all: fs flash
