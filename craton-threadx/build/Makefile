## Copyright (C) 2013-2015 Autotalks Ltd.

# To build the example outside of the SDK directory please change SDK_DIR
# to the path of the SDK directory

SDK_DIR := ../../..

include $(SDK_DIR)/build/rules.mk
include $(SDK_DIR)/build/craton.mk
include $(SDK_DIR)/build/u-boot.mk

.DEFAULT_GOAL := arm/img/build-example.img

# Target board
BOARD := atk22017c

# Global C/C++ compilation flags
CFLAGS := -Wall -Wextra -Werror -g -Os

# Global C++ compilation flags
CXXFLAGS := -std=c++11

# Global C compilation flags
CCFLAGS := -std=c99

# Compilation flags for specific environment
CFLAGS.arc1 := -D_DEFINED_ONLY_FOR_ARC1=1

# Compilation flags for specific object files
CFLAGS.arc1/obj/main.o := -D_INSERT_DELAY
CFLAGS.arc2/obj/main.o := -D_DO_NOTHING

# Global link flags
LDFLAGS := -lcraton -lstdc++ -lm

# Link flags for specific environment (commented out intentionally)
## LDFLAGS.arm :=

# Link flags for specific executable (commented out intentionally)
## LDFLAGS.arm/bin/build-example :=

# ARM executable that uses C++ code
arm/bin/build-example: \
	$(SDK_DIR)/board/$(BOARD)/arm/lib/ref-sys.o \
	arm/obj/main.o \
	arm/obj/unit.o

# ARC1 static library with a single (C++) object file
arc1/lib/libexample.a: \
	arc1/obj/unit.o

# ARC1 executable that uses a locally defined static library
arc1/bin/example: \
	$(SDK_DIR)/board/$(BOARD)/arc1/lib/ref-sys.o \
	arc1/obj/main.o \
	arc1/lib/libexample.a

# Note: if libexample.a was built externally and assumed to exist,
# then it should have been referenced via:
## LDFLAGS.arc1/bin/example := -lexample

# ARC2 executable
arc2/bin/example: \
	$(SDK_DIR)/board/$(BOARD)/arc2/lib/ref-sys.o \
	arc2/obj/main.o

ifeq ($(CRATON_NO_ARC),)

# Link ARC image data files with ARM executable
arm/bin/build-example: \
	arm/obj/example.arc1.o \
	arm/obj/example.arc2.o

endif # CRATON_NO_ARC
