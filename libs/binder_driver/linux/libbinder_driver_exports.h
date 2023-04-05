#pragma once

#if defined(WIN32) || defined(_MSC_VER)

#if defined(LIBBINDERDRIVER_EXPORTS_IMPL)
#define LIBBINDERDRIVER_EXPORTS __declspec(dllexport)
#else
#define LIBBINDERDRIVER_EXPORTS __declspec(dllimport)
#endif // defined(LIBBINDERDRIVER_EXPORTS_IMPL)

#else // defined(WIN32)
#if defined(LIBBINDERDRIVER_EXPORTS_IMPL)
#define LIBBINDERDRIVER_EXPORTS __attribute__((visibility("default")))
#else
#define LIBBINDERDRIVER_EXPORTS
#endif // defined(LIBBINDERDRIVER_EXPORTS_IMPL)
#endif

