## Copyright (C) 2013-2015 Autotalks Ltd.

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/craton.mk
include $(SDK_DIR)/build/u-boot.mk

ifeq ($(CRATON_NO_ARC),)

.DEFAULT_GOAL := arm/img/imq-example.img

CFLAGS := -Wall -Wextra -Werror -g -Os -std=c99
LDFLAGS := -lcraton -lm
BOARD := atk22017c

arm/bin/imq-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/imq-echo-server.o

arc1/bin/imq-example: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/imq-client.o

arc2/bin/imq-example: \
	$(SDK_DIR)/board/$(BOARD)/arc2/lib/ref-sys.o \
	arc2/obj/imq-client.o

arm/bin/imq-example: \
	arm/obj/imq-example.arc1.o \
	arm/obj/imq-example.arc2.o

else # CRATON_NO_ARC

.DEFAULT_GOAL := note
.PHONY: note
note:
	@echo 'imq-example: IMQ not supported in single-core SDK'

endif # CRATON_NO_ARC
