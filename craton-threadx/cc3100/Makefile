## Copyright (C) 2015 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/craton.mk
include $(SDK_DIR)/build/u-boot.mk

# WLAN examples
TARGET_EXAMPLE = arm/img/$(BOARD)-wlan-example.img

CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -g -Os
LDFLAGS := -lcraton -lm
BOARD := atk22010

.DEFAULT_GOAL := all

all: \
	arm/img/wifi-example.img 

arm/bin/wifi-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/wifi_http_srv_cmd.o \
	arm/obj/wifi_p2p_cmd.o \
	arm/obj/wifi_udp_cmd.o \
	arm/obj/wifi_tcp_cmd.o \
	arm/obj/wifi_ap_cmd.o \
	arm/obj/http_server/main.o \
	arm/obj/p2p/main.o \
	arm/obj/udp_socket/main.o \
	arm/obj/tcp_socket/main.o \
	arm/obj/getting_started_with_wlan_ap/main.o \
	arm/obj/wifi-example.o

arc1/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc1/bin/ref
	$(COPY)

arc2/bin/ref: $(SDK_DIR)/board/$(BOARD)/arc2/bin/ref
	$(COPY)

ifeq ($(CRATON_NO_ARC),)

arm/bin/wifi-example: arm/obj/ref.arc1.o arm/obj/ref.arc2.o

endif # CRATON_NO_ARC
