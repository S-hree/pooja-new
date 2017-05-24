## Copyright (C) 2013-2016 Autotalks Ltd.

SDK_DIR := .

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/host.mk

libsectonhost := $(HOST)/lib/libsectonhost.a
.DEFAULT_GOAL := $(libsectonhost)
c_files := $(shell find -L src -name '*.c')
$(libsectonhost): $(patsubst %.c,$(HOST)/obj/%.o,$(c_files))

# Set C compilation flags
CFLAGS = -std=gnu99               \
  -Wall -Wextra -Werror           \
  -Wno-missing-field-initializers \
  -Wno-unused-parameter           \
  -Wno-sign-compare               \
  $(extra_cflags)                 \
  -Isrc/include                   \
  -D_TR_MODULE=\"$(notdir $<)\"   \
  -O3

# LibTom{Math,Crypt} special compilation settings
libtom_cflags := -DLTC_SOURCE \
  -Wno-char-subscripts \
  -Wno-unused-variable \
  -Wno-unused-but-set-variable \
  -Wno-format \
  -Wno-implicit-function-declaration \
  -Wno-error=pointer-sign

$(HOST)/obj/src/libtommath/%.o: extra_cflags := $(libtom_cflags)
$(HOST)/obj/src/libtomcrypt/%.o: extra_cflags := $(libtom_cflags)
