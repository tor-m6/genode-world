SRC_GO_PKG := github.com/docker/docker/cmd/dockerd

#
# should prepare goroot port per each ARCH before!
# checked: x86 linux/linux; arm_v8a hw/virt_qemu_arm_v8a

GOPKG_MANUAL := yes
GO_GIT_URL := https://github.com/tor-m6/moby.git
GO_GIT_BRANCH := origin/master
GO_GIT_CHECKOUTDIR := src/github.com/docker/docker

GOBUILD_VERBOSE := -v

GOBUILD_FLAGS = -tags "no_btrfs no_cri no_devmapper inno static_build" -work -gccgoflags -Wl,--allow-multiple-definition

#GOBUILD_REBUILD_ALL = yes
# GO_BUILD_LIB = no
# DO_GET = no

include $(REP_DIR)/lib/mk/gobuild.inc

# for network apps
LIBS   += net

CC_CXX_WARN_STRICT =
