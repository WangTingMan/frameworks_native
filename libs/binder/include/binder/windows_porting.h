#pragma once
#include <cstdint>
#include <windows.h>
#include <corecrt_io.h>

#ifndef LIBBINDER_IMPLEMENTATION
#error "Only internal used"
#endif

#ifndef HAVE_MMAP   /* not true for MSVC, but may be true for msys */
#define MAP_FAILED  0
#define MREMAP_FIXED  2  /* the value in linux, though it doesn't really matter */
/* These, when combined with the mmap invariants below, yield the proper action */
#define PROT_READ      PAGE_READWRITE
#define PROT_WRITE     PAGE_READWRITE
#define MAP_ANONYMOUS  MEM_RESERVE
#define MAP_PRIVATE    MEM_COMMIT
#define MAP_SHARED     MEM_RESERVE   /* value of this #define is 100% arbitrary */
#endif

#ifndef O_SYNC
#define O_SYNC 0
#endif

uint32_t getpagesize();

uint32_t getuid();

