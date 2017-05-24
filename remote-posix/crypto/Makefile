## Copyright (C) 2013-2015 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/host.mk

LIB_DIR := $(SDK_DIR)/$(HOST)/lib
CFLAGS := -std=gnu99 -pedantic -Wall -Wextra -Werror -g -O3
LDFLAGS := -L$(LIB_DIR) -lsectonhost

.DEFAULT_GOAL := all

all: \
	$(HOST)/bin/ecdsa-example \
	$(HOST)/bin/ecies-example \
	$(HOST)/bin/aes-example \
	$(HOST)/bin/ecdsa-benchmark

$(HOST)/bin/ecdsa-example: \
	$(HOST)/obj/ecdsa-example.o

$(HOST)/bin/ecies-example: \
	$(HOST)/obj/ecies-example.o

$(HOST)/bin/aes-example: \
	$(HOST)/obj/aes-example.o

$(HOST)/bin/ecdsa-benchmark: \
	$(HOST)/obj/ecdsa-benchmark.o
