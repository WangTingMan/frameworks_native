#pragma once
#include <cstdint>
#include <atomic>
#include <mutex>

#ifndef __u32
#define __u32 uint32_t
#endif

#ifndef __u64
#define __u64 uint64_t
#endif

#ifndef __u8
#define __u8 uint8_t
#endif

#ifndef __s32
#define __s32 int32_t
#endif

#ifdef BINDER_IPC_32BIT
typedef __u32 binder_size_t;
typedef __u32 binder_uintptr_t;
#else
typedef __u64 binder_size_t;
typedef __u64 binder_uintptr_t;
#endif

using u8 = uint8_t;
using kuid_t = int32_t;
using kgid_t = int32_t;
using umode_t = int16_t;
using u32 = uint32_t;
using u64 = uint64_t;

struct list_head {
    struct list_head* next, * prev;
};

struct hlist_head {
    struct hlist_node* first;
};

struct hlist_node {
    struct hlist_node* next, ** pprev;
};

struct rb_node {
    unsigned long  __rb_parent_color;
    struct rb_node* rb_right;
    struct rb_node* rb_left;
} /*__attribute__((aligned(sizeof(long))))*/;
/* The alignment might seem pointless, but allegedly CRIS needs it */

struct rb_root {
    struct rb_node* rb_node;
};

typedef struct refcount_struct {
    std::atomic_int32_t refs;
} refcount_t;

struct wait_queue_head {
    std::mutex          lock;
    struct list_head    head;
};

typedef struct wait_queue_head wait_queue_head_t;
