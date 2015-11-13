define build_obj
obj=$$(BUILD_OUT)/$$(dir $(1))$(2)-$$(basename $$(notdir $(1))).o
dep:=$$(BUILD_OUT)/$$(dir $(1))$(2)-$$(basename $$(notdir $(1))).dep
-include $$(dep)
$$(obj): $$(CONFIG_FILE)
$$(obj): depfile:=$$(dep)
$$(obj): BUILD_CC:=$$(BUILD_CC)
$$(obj): BUILD_TAG:=$$(BUILD_TAG)
$$(obj): BUILD_GLOBAL_CFLAGS:=$$(BUILD_GLOBAL_CFLAGS)
$$(obj): $(1)
	$(Q)$(INFO) $$(BUILD_TAG) CC $$< '->' $$@
	$$(Q)$$(MKDIR) $$(dir $$@)
	$$(Q)$$(BUILD_CC) $$(BUILD_GLOBAL_CFLAGS) $(3) -c -MMD -MF $$(depfile) -o $$@ $$<
endef

