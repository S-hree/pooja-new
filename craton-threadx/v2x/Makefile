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

.DEFAULT_GOAL := arm/img/v2x-example.img

arm/bin/v2x-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/v2x-example.o

arc1/bin/v2x-example: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/v2x-example.o

arc2/bin/v2x-example: \
	$(SDK_DIR)/board/$(BOARD)/arc2/lib/ref-sys.o \
	arc2/obj/v2x-example.o

ifeq ($(CRATON_NO_ARC),)

arm/bin/v2x-example: \
	arm/obj/v2x-example.arc1.o \
	arm/obj/v2x-example.arc2.o

endif # CRATON_NO_ARC
