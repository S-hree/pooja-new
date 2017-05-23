# Copyright (C) 2015 Autotalks Ltd.

# Compatible compiler types
CRATON_ARM_GCC_TYPE := arm-none-eabi
CRATON_ARC_GCC_TYPE := arc-elf32

# Compatible compiler builds
CRATON_ARM_GCC_BUILD := '.* 4\.8\.4 20140725 .*\[atk-1\.0\.0-rel\]'
CRATON_ARC_GCC_BUILD := '(.* arc-2014\.08-rc1-4\.0) 4\.8\.0'

# List of CRATON architectures and  build environments
CRATON_ARCH_LIST := ARM
CRATON_ENV_NAMES := arm
ifeq ($(CRATON_NO_ARC),)
CRATON_ARCH_LIST += ARC
CRATON_ENV_NAMES += arc1 arc2
craton_lscript_prefix ?= craton
else
craton_lscript_prefix ?= craton-sc
endif

# Check compiler builds for all architectures
$(foreach a,$(CRATON_ARCH_LIST),$(if $(shell \
	$(CRATON_$a_TOOLSET_PATH)$(CRATON_$a_GCC_TYPE)-gcc --version | \
		grep -q $(CRATON_$a_GCC_BUILD) && echo y),, \
	$(error $(CRATON_$a_TOOLSET_PATH) not compatible with this SDK)))

# GNU toolset prefixes per environment
TOOLSET.arm := $(CRATON_ARM_TOOLSET_PATH)$(CRATON_ARM_GCC_TYPE)-
TOOLSET.arc1 := $(CRATON_ARC_TOOLSET_PATH)$(CRATON_ARC_GCC_TYPE)-
TOOLSET.arc2 := $(CRATON_ARC_TOOLSET_PATH)$(CRATON_ARC_GCC_TYPE)-

# Build environment specific directories
craton_env_inc ?= $(SDK_DIR)/release
craton_env_lib ?= $(SDK_DIR)/release

# CRATON compiler flags per environment
$(foreach e,$(CRATON_ENV_NAMES),$(eval craton_cflags.$e := \
	$(shell cat $(craton_env_lib)/$e/compiler.flags)))

# Define CRATON build configuration for environment $1
define craton_build_config
BUILTIN_CFLAGS.$1 := $(craton_cflags.$1)
BUILTIN_CFLAGS.$1 += -I$(craton_env_inc)/$1/include
BUILTIN_LDFLAGS.$1 := $(craton_cflags.$1)
BUILTIN_LDFLAGS.$1 += -L$(craton_env_lib)/$1/lib
BUILTIN_LDFLAGS.$1 += -T $(craton_lscript_prefix)-$1.ld
endef # craton_build_config

# Define rules and commands per CRATON build environment
$(foreach e,$(CRATON_ENV_NAMES), \
	$(eval $(call build_rules,$e)) \
	$(eval $(call build_commands.gnu,$e)) \
	$(eval $(call craton_build_config,$e)))

# Don't use built-in crt0 on ARM
BUILTIN_LDFLAGS.arm += -nostartfiles

# Convert ARC flat executable to ARM R/O data object
ifeq ($(CRATON_NO_ARC),)
blob_to_arm_obj := $(SDK_DIR)/build/blob-to-arm-obj $(TOOLSET.arm)
CMD.ARC1_BIN_TO_ARM_OBJ = $(blob_to_arm_obj) _arc1_image $< $@
CMD.ARC2_BIN_TO_ARM_OBJ = $(blob_to_arm_obj) _arc2_image $< $@

$(BUILD_DIR)/arm/obj/%.arc1.o: $(BUILD_DIR)/arc1/img/%.bin
	$(call run_cmd,ARC1_BIN_TO_ARM_OBJ)

$(BUILD_DIR)/arm/obj/%.arc2.o: $(BUILD_DIR)/arc2/img/%.bin
	$(call run_cmd,ARC2_BIN_TO_ARM_OBJ)
endif
