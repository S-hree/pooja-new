## Copyright (C) 2013-2016 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/craton.mk
include $(SDK_DIR)/build/u-boot.mk

CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -g -Os
LDFLAGS := -lcraton -lm
BOARD := atk22017c

.DEFAULT_GOAL := all

all: \
	arm/img/udp-receive-example.img \
	arm/img/nx-bsd-udp-receive-example.img \
	arm/img/nx-raw-packet-receive-example.img \
	arm/img/http-example.img

arm/bin/udp-receive-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/udp-receive-example.o

arm/bin/nx-bsd-udp-receive-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/nx-bsd-udp-receive-example.o

arm/bin/nx-raw-packet-receive-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/nx-raw-packet-receive-example.o

arm/bin/http-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/http-example.o

arc1/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc1/bin/ref
	$(COPY)

arc2/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc2/bin/ref
	$(COPY)

ifeq ($(CRATON_NO_ARC),)

arm/bin/udp-receive-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

arm/bin/nx-bsd-udp-receive-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

arm/bin/nx-raw-packet-receive-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

arm/bin/http-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

endif # CRATON_NO_ARC
