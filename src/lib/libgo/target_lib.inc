PKG_DIR = $(call select_from_ports,libgo)/src/lib/gcc/$(TARGET)
LD_OPT_NOSTDLIB := -nostdlib -Wl,-nostdlib
MAKE_TARGET := all

# add libc include to INC_DIR
include $(call select_from_repositories,lib/mk/libc-common.inc)

CONFIGURE_ARGS +=	--srcdir=$(PKG_DIR)/ \
			--cache-file=./config.cache \
			--disable-multilib \
			--disable-shared \
			--disable-libada \
			--with-gnu-as \
			--with-gnu-ld \
			--disable-tls \
			--disable-threads \
			--disable-hosted-libstdcxx \
			--enable-multiarch \
			--disable-sjlj-exceptions \
			--enable-languages=c,ada,c++,go,lto \
			--disable-option-checking

INSTALL_TARGET :=
INSTALL_TAR_ARCHIVE :=

include $(call select_from_repositories,mk/noux.mk)

H_INSTALL_STEP ?= find $(PKG_DIR)/ -name '*.h' -exec cp -fLp {} $(BUILD_BASE_DIR)/var/libcache/$(TARGET)/include/$(CACHE_INCLUDE) \;

installed.tag:
	@$(MSG_INST)$(TARGET)
	$(VERBOSE)mkdir -p $(BUILD_BASE_DIR)/var/libcache/$(TARGET)/include/$(CACHE_INCLUDE)
	$(VERBOSE)ln -sf $(BUILD_BASE_DIR)/lib/$(TARGET)/.libs/$(TARGET).a $(BUILD_BASE_DIR)/var/libcache/$(TARGET)/$(TARGET).a
	$(VERBOSE)ln -sf $(BUILD_BASE_DIR)/lib/$(TARGET)/$(TARGET).la $(BUILD_BASE_DIR)/var/libcache/$(TARGET)/$(TARGET).la
	@touch $@
	@echo ".... remove built.tag and installed.tag ....."
	@rm $(BUILD_BASE_DIR)/lib/$(TARGET)/built.tag
	@rm $(BUILD_BASE_DIR)/lib/$(TARGET)/installed.tag
	@echo ".... copy h-files for to var/libcache/$(TARGET)/include ....."
	$(VERBOSE)$(H_INSTALL_STEP)
	$(VERBOSE)$(EXEC_INSTALL_STEP)
