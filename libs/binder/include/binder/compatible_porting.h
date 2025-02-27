#pragma once

#include <stdint.h>

#ifndef _DO_NOT_DEFINE_IOVEC_
#ifndef _IOVEC_DEFINED_
struct iovec
{
    void* iov_base;
    size_t iov_len;
};
#endif
#define _IOVEC_DEFINED_
#endif

class fake_file_descriptor
{

};

