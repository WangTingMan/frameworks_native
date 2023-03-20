#pragma once

#if defined(WIN32) || defined(_MSC_VER)

#if defined(LIBBINDER_IMPLEMENTATION)
#define LIBBINDER_EXPORT __declspec(dllexport)
#else
#define LIBBINDER_EXPORT __declspec(dllimport)
#endif // defined(LIBBINDER_EXPORT)

#else // defined(WIN32)
#if defined(LIBBINDER_IMPLEMENTATION)
#define LIBBINDER_EXPORT __attribute__((visibility("default")))
#else
#define LIBBINDER_EXPORT
#endif // defined(LIBBINDER_IMPLEMENTATION)
#endif

