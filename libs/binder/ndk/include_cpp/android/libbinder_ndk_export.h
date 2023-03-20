#pragma once

#if defined(WIN32) || defined(_MSC_VER)

#if defined(LIBBINDER_NDK_IMPLEMENTATION)
#define LIBBINDER_NDK_EXPORT __declspec(dllexport)
#else
#define LIBBINDER_NDK_EXPORT __declspec(dllimport)
#endif  // defined(LIBBINDER_NDK_EXPORT)

#ifndef __INTRODUCED_IN
#define __INTRODUCED_IN(a)
#endif

#else  // defined(WIN32)
#if defined(LIBBINDER_NDK_IMPLEMENTATION)
#define LIBBINDER_NDK_EXPORT __attribute__((visibility("default")))
#else
#define LIBBINDER_NDK_EXPORT
#endif  // defined(LIBBINDER_IMPLEMENTATION)
#endif
