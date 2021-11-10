TARGET = go-ping
SRC_GO = ping.go

MPRG_REL_DIR := $(subst $(REP_DIR)/src/,,$(PRG_DIR))
MPRG_REL_DIR := $(MPRG_REL_DIR:/=)
GO_PKG_BUILD := $(BUILD_BASE_DIR)/$(MPRG_REL_DIR)
SRCPATH      := $(REP_DIR)/src/$(MPRG_REL_DIR)/src

include $(PRG_DIR)/gobase.mk

LIBS   = base libc libm libatomic libbacktrace libffi libgo libgo_support stdcxx

CC_CXX_WARN_STRICT =
