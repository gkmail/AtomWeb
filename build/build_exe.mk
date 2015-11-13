BUILD_TARGET:=$(TARGET)$(BUILD_EXE_SUFFIX)
BUILD_PREFIX:=$(subst .,_,$(BUILD_TARGET))
BUILD_TARGET_FULL:=$(BUILD_OUT)/$(BUILD_TARGET)

TARGET_LIB_FLAGS+=$(foreach lib,$(DLIBS),-l$(patsubst lib%,%,$(lib)))
TARGET_DEPS+=$(foreach lib,$(DLIBS),$(BUILD_OUT)/$(lib)$(BUILD_DLIB_SUFFIX))

ifneq ($(AUTO_BUILD), 0)
all: $(BUILD_TARGET_FULL)
endif

OBJS:=$(foreach src,$(SRCS),$(BUILD_OUT)/$(dir $(src))$(BUILD_PREFIX)-$(basename $(notdir $(src))).o)
OBJS+=$(foreach lib,$(SLIBS),$(BUILD_OUT)/$(lib)$(BUILD_SLIB_SUFFIX))

$(foreach src,$(SRCS),$(eval $(call build_obj,$(src),$(BUILD_PREFIX),$(CFLAGS))))

$(BUILD_TARGET_FULL): BUILD_LDFLAGS:=-L$(OUT) $(TARGET_LIB_FLAGS) $(LDFLAGS) $(BUILD_GLOBAL_LDFLAGS)
$(BUILD_TARGET_FULL): BUILD_TAG:=$(BUILD_TAG)
$(BUILD_TARGET_FULL): BUILD_CC:=$(BUILD_CC)
$(BUILD_TARGET_FULL): BUILD_STRIP:=$(BUILD_STRIP)
$(BUILD_TARGET_FULL): PWD:=$(shell pwd)
$(BUILD_TARGET_FULL): $(OBJS)
	$(Q)$(INFO) $(BUILD_TAG) CC '->' $@
	$(Q)$(BUILD_CC) -o $@ $^ $(BUILD_LDFLAGS)
ifneq ($(DEBUG),1)
	$(Q)$(BUILD_STRIP) $@
endif
	$(Q)$(INFO) GEN $@.sh
	$(Q)$(ECHO) '#!/bin/bash' > $@.sh
	$(Q)$(ECHO) "LD_LIBRARY_PATH=$(PWD)/$(OUT) $(PWD)/$@ "'$$@' >> $@.sh
	$(Q)$(CHMOD) 755 $@.sh

ifneq ($(TARGET_DEPS),)
$(OBJS): $(TARGET_DEPS)
endif

CLEAN_TARGETS+=$(BUILD_TARGET_FULL) $(OBJS)

include build/clear_env.mk
