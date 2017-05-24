## Copyright (C) 2015-2016 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/host.mk

LIB_DIR := $(SDK_DIR)/$(HOST)/lib
CFLAGS := -std=gnu99 -pedantic -Wall -Wextra -Werror -g -O3
LDFLAGS := -L$(LIB_DIR) -lsectonhost -lm

.DEFAULT_GOAL := all

all: \
	$(HOST)/bin/gnss-teseo-fw-update-example

$(HOST)/bin/gnss-teseo-fw-update-example: \
	$(HOST)/obj/gnss-teseo-fw-update-example.o
