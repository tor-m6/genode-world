include $(call select_from_repositories,src/lib/libgo/target_lib.inc)

#
# Make the configure linking test succeed
#
Makefile: dummy_libs

LDFLAGS += -L$(PWD) 

.SECONDARY: dummy_libs
dummy_libs: libpthread.a

libpthread.a:
	$(VERBOSE)$(AR) -rc $@
