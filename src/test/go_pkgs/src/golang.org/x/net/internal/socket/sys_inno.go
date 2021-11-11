// Copyright 2017 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

//go:build inno
// +build inno

package socket

import (
	"syscall"
)

type (
	_C_int   int32
)

// Automatically generated wrapper for getsockopt/getsockopt
//extern getsockopt
func c_getsockopt(s _C_int, level _C_int, name _C_int, val *byte, vallen *syscall.Socklen_t) _C_int
func getsockopt(s uintptr, level, name int, b []byte) (int, error) {
// func getsockopt(s int, level int, name int, val unsafe.Pointer, vallen *syscall.Socklen_t) (err error) {
        syscall.Entersyscall()
		var vallen syscall.Socklen_t
        _r := c_getsockopt(_C_int(s), _C_int(level), _C_int(name), &b[0], &vallen)
        var errno syscall.Errno
        setErrno := false
        if _r < 0 {
                errno = syscall.GetErrno()
                setErrno = true
        }
        syscall.Exitsyscall()
		var err error
        if setErrno {
                err = errno
        }
        return int(vallen),err
}

// Automatically generated wrapper for setsockopt/setsockopt
//go:noescape
//extern setsockopt
func c_setsockopt(s _C_int, level _C_int, optname _C_int, val *byte, vallen syscall.Socklen_t) _C_int
func setsockopt(s uintptr, level, name int, b []byte) (err error) {
	syscall.Entersyscall()
	var vallen syscall.Socklen_t
	_r := c_setsockopt(_C_int(s), _C_int(level), _C_int(name), &b[0], syscall.Socklen_t(vallen))
	var errno syscall.Errno
	setErrno := false
	if _r < 0 {
		errno = syscall.GetErrno()
		setErrno = true
	}
	syscall.Exitsyscall()
	if setErrno {
		err = errno
	}
	return err
}


// func recvmsg(s uintptr, h *msghdr, flags int) (int, error) {
// 	n, _, errno := syscall.Syscall(syscall.SYS_RECVMSG, s, uintptr(unsafe.Pointer(h)), uintptr(flags))
// 	return int(n), errnoErr(errno)
// }

// // func sendmsg(s uintptr, h *msghdr, flags int) (int, error) {
// // 	n, _, errno := syscall.Syscall(syscall.SYS_SENDMSG, s, uintptr(unsafe.Pointer(h)), uintptr(flags))
// // 	return int(n), errnoErr(errno)
// // }

// Automatically generated wrapper for sendmsg/sendmsg
//extern sendmsg
func c_sendmsg(s _C_int, msg *msghdr, flags _C_int) syscall.Ssize_t
func sendmsg(s uintptr, h *msghdr, flags int) (n int, err error) {
// func sendmsg(s int, msg *Msghdr, flags int) (n int, err error) {
        syscall.Entersyscall()
        _r := c_sendmsg(_C_int(s), h, _C_int(flags))
        n = (int)(_r)
        var errno syscall.Errno
        setErrno := false
        if _r < 0 {
                errno = syscall.GetErrno()
                setErrno = true
        }
        syscall.Exitsyscall()
        if setErrno {
                err = errno
        }
        return
}

// Automatically generated wrapper for recvmsg/recvmsg
//extern recvmsg
func c_recvmsg(s _C_int, msg *msghdr, flags _C_int) syscall.Ssize_t
func recvmsg(s uintptr, h *msghdr, flags int) (n int, err error) {
// func recvmsg(s int, msg *Msghdr, flags int) (n int, err error) {
        syscall.Entersyscall()
        _r := c_recvmsg(_C_int(s), h, _C_int(flags))
        n = (int)(_r)
        var errno syscall.Errno
        setErrno := false
        if _r < 0 {
                errno = syscall.GetErrno()
                setErrno = true
        }
        syscall.Exitsyscall()
        if setErrno {
                err = errno
        }
        return
}
