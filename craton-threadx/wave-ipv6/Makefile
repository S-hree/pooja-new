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
	arm/img/wave-ipv6-example.img   \
	arm/img/wave-ipv6-client-example.img \
	arm/img/wave-ipv6-server-example.img 

arm/bin/wave-ipv6-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/wave-ipv6-example.o

arm/bin/wave-ipv6-client-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/wave-ipv6-client-example.o

arm/bin/wave-ipv6-server-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/wave-ipv6-server-example.o	

arc1/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc1/bin/ref
	$(COPY)

arc2/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc2/bin/ref
	$(COPY)

ifeq ($(CRATON_NO_ARC),)

arm/bin/wave-ipv6-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

arm/bin/wave-ipv6-client-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

arm/bin/wave-ipv6-server-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

endif # CRATON_NO_ARC
