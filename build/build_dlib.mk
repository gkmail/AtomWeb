BUILD_TARGET:=$(TARGET)$(BUILD_DLIB_SUFFIX)
BUILD_PREFIX:=$(subst .,_,$(BUILD_TARGET))
BUILD_TARGET_FULL:=$(BUILD_OUT)/$(BUILD_TARGET)

ifneq ($(AUTO_BUILD), 0)
all: $(BUILD_TARGET_FULL)
endif

OBJS:=$(foreach src,$(SRCS),$(BUILD_OUT)/$(dir $(src))$(BUILD_PREFIX)-$(basename $(notdir $(src))).o)

$(foreach src,$(SRCS),$(eval $(call build_obj,$(src),$(BUILD_PREFIX),$(BUILD_CFLAGS) -fPIC)))

$(BUILD_TARGET_FULL): BUILD_LDFLAGS:=$(LDFLAGS) $(BUILD_GLOBAL_LDFLAGS)
$(BUILD_TARGET_FULL): BUILD_CC:=$(BUILD_CC)
$(BUILD_TARGET_FULL): BUILD_STRIP:=$(BUILD_STRIP)
$(BUILD_TARGET_FULL): BUILD_TAG:=$(BUILD_TAG)
$(BUILD_TARGET_FULL): $(OBJS)
	$(Q)$(INFO) $(BUILD_TAG) CC '->' $@
	$(Q)$(BUILD_CC) -o $@ --shared $(BUILD_LDFLAGS) $^
ifneq ($(DEBUG),1)
	$(Q)$(BUILD_STRIP) $@
endif

CLEAN_TARGETS+=$(BUILD_TARGET_FULL) $(OBJS)

include build/clear_env.mk
