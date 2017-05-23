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

.DEFAULT_GOAL := arm/img/v2x-emulator-example.img

arm/bin/v2x-emulator-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/v2x-emulator-over-udp-example.o 

arc1/bin/v2x-emulator-example: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/v2x-service-user.o

arc2/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc2/bin/ref
	$(COPY)

ifeq ($(CRATON_NO_ARC),)

arm/bin/v2x-emulator-example: \
	arm/obj/v2x-emulator-example.arc1.o \
	arm/obj/ref.arc2.o

else

arm/bin/v2x-emulator-example: \
	arm/obj/v2x-service-user.o

endif # CRATON_NO_ARC
