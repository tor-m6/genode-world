SRC_GO_PKG := github.com/opencontainers/runc

#
# should prepare goroot port per each ARCH before!
# checked: x86 linux/linux; arm_v8a hw/virt_qemu_arm_v8a

#GOBUILD_VERBOSE := -x -v

GOPKG_MANUAL := yes
GO_GIT_URL := https://github.com/tor-m6/runc.git
GO_GIT_BRANCH := main

GOBUILD_FLAGS = -tags inno

include $(REP_DIR)/lib/mk/gobuild.inc

# for network apps
LIBS   += net

CC_CXX_WARN_STRICT =
