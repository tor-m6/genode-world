LIBS += libatomic libbacktrace libffi libgo_support
#CC_OLEVEL = -O0
H_INSTALL_STEP := @true
EXEC_INSTALL_STEP =	ln -sf $(BUILD_BASE_DIR)/lib/libgo/libgobegin.a $(LIB_CACHE_DIR)/libgo/libgobegin.a; \
		ln -sf $(BUILD_BASE_DIR)/lib/libgo/libgolibbegin.a $(LIB_CACHE_DIR)/libgo/libgolibbegin.a

include $(call select_from_repositories,src/lib/libgo/target_lib.inc)
