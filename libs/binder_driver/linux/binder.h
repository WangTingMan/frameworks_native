#pragma once
#include "base_types.h"

#include <linux/libbinder_driver_exports.h>

#define B_PACK_CHARS(c1,c2,c3,c4) ((((c1) << 24)) | (((c2) << 16)) | (((c3) << 8)) | (c4))
#define B_TYPE_LARGE 0x85

#ifndef SCHED_NORMAL
#define SCHED_NORMAL 0
#endif

#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC 1030
#endif

#ifndef pid_t
#define pid_t int
#endif

#ifndef uid_t
#define uid_t int32_t
#endif

enum {
    BINDER_TYPE_BINDER = B_PACK_CHARS('s', 'b', '*', B_TYPE_LARGE),
    BINDER_TYPE_WEAK_BINDER = B_PACK_CHARS('w', 'b', '*', B_TYPE_LARGE),
    BINDER_TYPE_HANDLE = B_PACK_CHARS('s', 'h', '*', B_TYPE_LARGE),
    BINDER_TYPE_WEAK_HANDLE = B_PACK_CHARS('w', 'h', '*', B_TYPE_LARGE),
    BINDER_TYPE_FD = B_PACK_CHARS('f', 'd', '*', B_TYPE_LARGE),
    BINDER_TYPE_FDA = B_PACK_CHARS('f', 'd', 'a', B_TYPE_LARGE),
    BINDER_TYPE_PTR = B_PACK_CHARS('p', 't', '*', B_TYPE_LARGE),
};

enum flat_binder_object_shifts {
    FLAT_BINDER_FLAG_SCHED_POLICY_SHIFT = 9,
};

enum flat_binder_object_flags {
    FLAT_BINDER_FLAG_PRIORITY_MASK = 0xff,
    FLAT_BINDER_FLAG_ACCEPTS_FDS = 0x100,
    FLAT_BINDER_FLAG_SCHED_POLICY_MASK = 3U << FLAT_BINDER_FLAG_SCHED_POLICY_SHIFT,
    FLAT_BINDER_FLAG_INHERIT_RT = 0x800,
    FLAT_BINDER_FLAG_TXN_SECURITY_CTX = 0x1000,
};

enum transaction_flags {
    TF_ONE_WAY = 0x01,
    TF_ROOT_OBJECT = 0x04,
    TF_STATUS_CODE = 0x08,
    TF_ACCEPT_FDS = 0x10,
    TF_CLEAR_BUF = 0x20,
};

#define CONFIG_ANDROID_VENDOR_OEM_DATA

#ifndef _IOC_WRITE
#define _IOC_WRITE 1U
#endif

#define _IOC_NRBITS 8
#define _IOC_NRSHIFT 0
#define _IOC_TYPEBITS 8
#define _IOC_SIZEBITS 14
#define _IOC_NONE   0U

#define _IOC_NRMASK	((1 << _IOC_NRBITS)-1)

#define _IOC_TYPESHIFT (_IOC_NRSHIFT + _IOC_NRBITS)
#define _IOC_SIZESHIFT (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT (_IOC_SIZESHIFT + _IOC_SIZEBITS)

#define _IOC(dir,type,nr,size) (((dir) << _IOC_DIRSHIFT) | ((type) << _IOC_TYPESHIFT) | ((nr) << _IOC_NRSHIFT) | ((size) << _IOC_SIZESHIFT))

#define _IOC_TYPECHECK(t) (sizeof(t))
#define _IOW(type,nr,size) _IOC(_IOC_WRITE, (type), (nr), (_IOC_TYPECHECK(size)))
#define _IO(type,nr)    _IOC(_IOC_NONE,(type),(nr),0)

/*
 * Direction bits _IOC_NONE could be 0, but OSF/1 gives it a bit.
 * And this turns out useful to catch old ioctl numbers in header
 * files for us.
 */
#define _IOC_NONE	1U
#define _IOC_READ	2U
#define _IOC_WRITE	4U

/* used to create numbers */
#define _IO(type,nr)		_IOC(_IOC_NONE,(type),(nr),0)
#define _IOR(type,nr,size)	_IOC(_IOC_READ,(type),(nr),sizeof(size))
#define _IOW(type,nr,size)	_IOC(_IOC_WRITE,(type),(nr),sizeof(size))
#define _IOWR(type,nr,size)	_IOC(_IOC_READ|_IOC_WRITE,(type),(nr),sizeof(size))
#define _IOC_NR(nr)		(((nr) >> _IOC_NRSHIFT) & _IOC_NRMASK)

struct binder_transaction_data {
    union {
        __u32 handle;
        binder_uintptr_t ptr;
    } target;
    binder_uintptr_t cookie;
    __u32 code;
    __u32 flags;
    pid_t sender_pid;
    uid_t sender_euid;
    binder_size_t data_size;
    binder_size_t offsets_size;
    union {
        struct {
            binder_uintptr_t buffer;
            binder_uintptr_t offsets;
        } ptr;
        __u8 buf[8];
    } data;
};

struct binder_ptr_cookie {
    binder_uintptr_t ptr;
    binder_uintptr_t cookie;
};

struct binder_pri_desc {
    __s32 priority;
    __u32 desc;
};

struct binder_handle_cookie {
    __u32 handle;
    binder_uintptr_t cookie;
};

struct binder_transaction_data_sg {
    struct binder_transaction_data transaction_data;
    binder_size_t buffers_size;
};

struct binder_transaction_data_secctx {
    struct binder_transaction_data transaction_data;
    binder_uintptr_t secctx;
};

struct binder_pri_ptr_cookie {
    __s32 priority;
    binder_uintptr_t ptr;
    binder_uintptr_t cookie;
};

struct binder_write_read {
    binder_size_t write_size;
    binder_size_t write_consumed;
    binder_uintptr_t write_buffer;
    binder_size_t read_size;
    binder_size_t read_consumed;
    binder_uintptr_t read_buffer;
};

struct binder_node_debug_info {
    binder_uintptr_t ptr;
    binder_uintptr_t cookie;
    __u32            has_strong_ref;
    __u32            has_weak_ref;
};

struct binder_node_info_for_ref {
    __u32            handle;
    __u32            strong_count;
    __u32            weak_count;
    __u32            reserved1;
    __u32            reserved2;
    __u32            reserved3;
};

/* Use with BINDER_VERSION, driver fills in fields. */
struct binder_version {
    /* driver protocol version -- increment with incompatible change */
    __s32       protocol_version;
};

/* This is the current protocol version. */
#ifdef BINDER_IPC_32BIT
#define BINDER_CURRENT_PROTOCOL_VERSION 7
#else
#define BINDER_CURRENT_PROTOCOL_VERSION 8
#endif

#define BINDER_WRITE_READ _IOWR('b', 1, struct binder_write_read)
#define BINDER_SET_IDLE_TIMEOUT _IOW('b', 3, __s64)
#define BINDER_SET_MAX_THREADS _IOW('b', 5, __u32)
#define BINDER_SET_IDLE_PRIORITY _IOW('b', 6, __s32)
#define BINDER_SET_CONTEXT_MGR _IOW('b', 7, __s32)
#define BINDER_THREAD_EXIT _IOW('b', 8, __s32)
#define BINDER_VERSION _IOWR('b', 9, struct binder_version)
#define BINDER_GET_NODE_DEBUG_INFO _IOWR('b', 11, struct binder_node_debug_info)
#define BINDER_GET_NODE_INFO_FOR_REF _IOWR('b', 12, struct binder_node_info_for_ref)
#define BINDER_SET_CONTEXT_MGR_EXT _IOW('b', 13, struct flat_binder_object)
#define BINDER_ENABLE_ONEWAY_SPAM_DETECTION _IOW('b', 16, __u32)

enum binder_driver_command_protocol {
    BC_TRANSACTION = _IOW('c', 0, struct binder_transaction_data),
    BC_REPLY = _IOW('c', 1, struct binder_transaction_data),
    BC_ACQUIRE_RESULT = _IOW('c', 2, __s32),
    BC_FREE_BUFFER = _IOW('c', 3, binder_uintptr_t),
    BC_INCREFS = _IOW('c', 4, __u32),
    BC_ACQUIRE = _IOW('c', 5, __u32),
    BC_RELEASE = _IOW('c', 6, __u32),
    BC_DECREFS = _IOW('c', 7, __u32),
    BC_INCREFS_DONE = _IOW('c', 8, struct binder_ptr_cookie),
    BC_ACQUIRE_DONE = _IOW('c', 9, struct binder_ptr_cookie),
    BC_ATTEMPT_ACQUIRE = _IOW('c', 10, struct binder_pri_desc),
    BC_REGISTER_LOOPER = _IO('c', 11),
    BC_ENTER_LOOPER = _IO('c', 12),
    BC_EXIT_LOOPER = _IO('c', 13),
    BC_REQUEST_DEATH_NOTIFICATION = _IOW('c', 14, struct binder_handle_cookie),
    BC_CLEAR_DEATH_NOTIFICATION = _IOW('c', 15, struct binder_handle_cookie),
    BC_DEAD_BINDER_DONE = _IOW('c', 16, binder_uintptr_t),
    BC_TRANSACTION_SG = _IOW('c', 17, struct binder_transaction_data_sg),
    BC_REPLY_SG = _IOW('c', 18, struct binder_transaction_data_sg),
};

#ifdef BR_FROZEN_REPLY
#undef BR_FROZEN_REPLY
#endif

#ifdef BR_ONEWAY_SPAM_SUSPECT
#undef BR_ONEWAY_SPAM_SUSPECT
#endif

enum binder_driver_return_protocol {
    BR_ERROR = (0x40000000 | (((long)sizeof(int32_t) & 0x7f) << 16) | (('r') << 8) | (0)),
    BR_OK = _IO('r', 1),
    BR_TRANSACTION_SEC_CTX = (0x40000000 | (((long)sizeof(struct binder_transaction_data_secctx) & 0x7f) << 16) | (('r') << 8) | (2)),
    BR_TRANSACTION = (0x40000000 | (((long)sizeof(struct binder_transaction_data) & 0x7f) << 16) | (('r') << 8) | (2)),
    BR_REPLY = (0x40000000 | (((long)sizeof(struct binder_transaction_data) & 0x7f) << 16) | (('r') << 8) | (3)),
    BR_ACQUIRE_RESULT = (0x40000000 | (((long)sizeof(int32_t) & 0x7f) << 16) | ((('r')) << 8) | (4)),
    BR_DEAD_REPLY = _IO('r', 5),
    BR_TRANSACTION_COMPLETE = _IO('r', 6),
    BR_INCREFS = (0x40000000 | (((long)sizeof(struct binder_ptr_cookie) & 0x7f) << 16) | (('r') << 8) | (7)),
    BR_ACQUIRE = (0x40000000 | (((long)sizeof(struct binder_ptr_cookie) & 0x7f) << 16) | (('r') << 8) | (8)),
    BR_RELEASE = (0x40000000 | (((long)sizeof(struct binder_ptr_cookie) & 0x7f) << 16) | (('r') << 8) | (9)),
    BR_DECREFS = (0x40000000 | (((long)sizeof(struct binder_ptr_cookie) & 0x7f) << 16) | (('r') << 8) | (10)),
    BR_ATTEMPT_ACQUIRE = (0x40000000 | (((long)sizeof(struct binder_pri_ptr_cookie) & 0x7f) << 16) | (('r') << 8) | (11)),
    BR_NOOP = _IO('r', 12),
    BR_SPAWN_LOOPER = _IO('r', 13),
    BR_FINISHED = _IO('r', 14),
    BR_DEAD_BINDER = (0x40000000 | (((long)sizeof(binder_uintptr_t) & 0x7f) << 16) | (('r') << 8) | (15)),
    BR_CLEAR_DEATH_NOTIFICATION_DONE = (0x40000000 | (((long)sizeof(binder_uintptr_t) & 0x7f) << 16) | (('r') << 8) | (16)),
    BR_FAILED_REPLY = 1073771025,
    BR_FROZEN_REPLY = 1073771026,
    BR_ONEWAY_SPAM_SUSPECT = (((1U) << (((0 + 8) + 8) + 14)) | ((('r')) << (0 + 8)) | (((19)) << 0) | ((0) << ((0 + 8) + 8))),
};

#ifdef CONFIG_ANDROID_VENDOR_OEM_DATA
#define ANDROID_VENDOR_DATA(n)		u64 android_vendor_data##n
#define ANDROID_VENDOR_DATA_ARRAY(n, s)	u64 android_vendor_data##n[s]
#endif

struct binder_object_header {
    __u32 type;
};

struct flat_binder_object {
    struct binder_object_header hdr;
    __u32 flags;
    union {
        binder_uintptr_t binder;
        __u32 binder_internal_handle;
    };
    binder_uintptr_t cookie;
};

namespace porting_binder
{
    LIBBINDERDRIVER_EXPORTS __u32 open_binder(const char*, ...);
    LIBBINDERDRIVER_EXPORTS void close_binder( __u32 handle );
    LIBBINDERDRIVER_EXPORTS __u32 fcntl_binder(__u32 handle, uint32_t to_operation, uint32_t parameters);
    LIBBINDERDRIVER_EXPORTS __u32 fcntl_binder(__u32 handle, uint32_t to_operation, void* parameters);
}

struct binder_fd_object {
    struct binder_object_header hdr;
    __u32 pad_flags;
    union {
        binder_uintptr_t pad_binder;
        __u32 fd;
    };
    binder_uintptr_t cookie;
};

struct binder_buffer_object {
    struct binder_object_header hdr;
    __u32 flags;
    binder_uintptr_t buffer;
    binder_size_t length;
    binder_size_t parent;
    binder_size_t parent_offset;
};

struct binder_fd_array_object {
    struct binder_object_header hdr;
    __u32 pad;
    binder_size_t num_fds;
    binder_size_t parent;
    binder_size_t parent_offset;
};
