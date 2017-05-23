## Copyright (C) 2015 Autotalks Ltd.

BIN_TO_UIMAGE := $(SDK_DIR)/build/arm-bin-to-uimage
CMD.BIN_TO_UIMAGE = $(BIN_TO_UIMAGE) $< $@ $(BIN_TO_UIMAGE_FLAGS)

$(BUILD_DIR)/arm/img/%.img: $(BUILD_DIR)/arm/img/%.bin
	$(call run_cmd,BIN_TO_UIMAGE)
