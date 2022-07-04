CACHE_INCLUDE := contrib
EXEC_INSTALL_STEP =	find $(BUILD_BASE_DIR)/lib/$(TARGET)/include/ -name '*.*' -exec cp -fLp {} $(BUILD_BASE_DIR)/var/libcache/$(TARGET)/include \;

include $(call select_from_repositories,src/lib/libgo/target_lib.inc)
