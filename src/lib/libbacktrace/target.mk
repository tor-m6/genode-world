EXT_OBJECTS += $(shell $(CC) $(CC_MARCH) -print-file-name=libgcc_eh.a)
#SHARED_LIBS += ld.lib.so

EXEC_INSTALL_STEP =	cp $(BUILD_BASE_DIR)/lib/$(TARGET)/gstdint.h $(LIB_CACHE_DIR)/$(TARGET)/include/

include $(call select_from_repositories,src/lib/libgo/target_lib.inc)
