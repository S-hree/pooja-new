## Copyright (C) 2013-2015 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/craton.mk
include $(SDK_DIR)/build/u-boot.mk

CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -g -Os
LDFLAGS := -lcraton -lm
BOARD := atk22017c

ifneq ($(CRATON_NO_ARC),) 

CFLAGS += -DFS_EXIST

endif

.DEFAULT_GOAL := all

all: \
	arm/img/ecdsa-example.img \
	arm/img/ecies-example.img \
	arm/img/aes-example.img \
	arm/img/ecdsa-benchmark.img \
	arm/img/secure-storage-example.img

arm/bin/ecdsa-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/ecdsa-example.o

arc1/bin/ecdsa-example: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/ecdsa-example.o

arc1/bin/ecies-example: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/ecies-example.o

arm/bin/ecies-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/ecies-example.o

arm/bin/aes-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/aes-example.o

arc1/bin/aes-example: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/aes-example.o  

arm/bin/ecdsa-benchmark: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/ecdsa-benchmark.o

arc1/bin/ecdsa-benchmark: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/ecdsa-benchmark.o

arm/bin/secure-storage-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/secure-storage-example.o

arc1/bin/secure-storage-example: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/secure-storage-example.o

arc2/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc2/bin/ref
	$(COPY)

ifeq ($(CRATON_NO_ARC),)


arm/bin/ecdsa-example: \
	arm/obj/ecdsa-example.arc1.o \
	arm/obj/ref.arc2.o

arm/bin/ecies-example: \
	arm/obj/ecies-example.arc1.o \
	arm/obj/ref.arc2.o

arm/bin/aes-example: \
	arm/obj/aes-example.arc1.o \
	arm/obj/ref.arc2.o

arm/bin/ecdsa-benchmark: \
	arm/obj/ecdsa-benchmark.arc1.o \
	arm/obj/ref.arc2.o

arm/bin/secure-storage-example: \
	arm/obj/secure-storage-example.arc1.o \
	arm/obj/ref.arc2.o

endif # CRATON_NO_ARC
