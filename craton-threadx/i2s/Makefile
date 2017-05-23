## Copyright (C) 2013-2015 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/craton.mk
include $(SDK_DIR)/build/u-boot.mk

CFLAGS := -std=gnu99 -pedantic -Wall -Wextra -Werror -g -Os
LDFLAGS := -lcraton -lm
BOARD := atk23010a

.DEFAULT_GOAL := arm/img/i2s-example.img

arm/bin/i2s-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/i2s-example.o

arc1/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc1/bin/ref
	$(COPY)

arc2/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc2/bin/ref
	$(COPY)

ifeq ($(CRATON_NO_ARC),)

arm/bin/i2s-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

endif # CRATON_NO_ARC
