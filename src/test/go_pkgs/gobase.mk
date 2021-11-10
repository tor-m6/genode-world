# Run from the root of the source tree.
#
#
include $(PRG_DIR)/Deps.inc
include $(PRG_DIR)/Golookups.inc

%.a:
	$(MSG_COMP)$@
	$(VERBOSE)mkdir -p $(basename $@)
	$(VERBOSE)$(GOLANG) $(CUSTOM_GO_FLAGS) -I. -c -fgo-pkgpath=$(basename $@) -o $(basename $@)/_go_.o ${lookup.$@}
	$(VERBOSE)$(AR) rc $@ $(basename $@)/_go_.o
	$(VERBOSE)ln -sf $(@F) $(@D)/lib$(@F)

ping.o: github.com/tatsushid/go-fastping.a golang.org/x/net/internal/iana.a golang.org/x/net/internal/socket.a golang.org/x/net/bpf.a golang.org/x/net/icmp.a golang.org/x/net/ipv4.a golang.org/x/net/ipv6.a golang.org/x/sys/internal/unsafeheader.a golang.org/x/sys/unix.a

EXT_OBJECTS += github.com/tatsushid/go-fastping.a golang.org/x/net/internal/iana.a golang.org/x/net/internal/socket.a golang.org/x/net/bpf.a golang.org/x/net/icmp.a golang.org/x/net/ipv4.a golang.org/x/net/ipv6.a golang.org/x/sys/internal/unsafeheader.a golang.org/x/sys/unix.a
