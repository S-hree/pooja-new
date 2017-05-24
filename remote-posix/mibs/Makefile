## Copyright (C) 2015 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/host.mk

LIB_DIR := $(SDK_DIR)/$(HOST)/lib
CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -g -O3
LDFLAGS := -L$(LIB_DIR) -lsectonhost

.DEFAULT_GOAL := $(HOST)/bin/mibs-example

$(HOST)/bin/mibs-example: $(HOST)/obj/mibs-example.o
