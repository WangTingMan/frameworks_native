#pragma once

/**
 * This is porting of libs\binder\include\binder\Parcel.h.
 * Since libs\binder\include\binder\Parcel.h is exported by libbinder.so
 * and libbinder.so will use libbinder_driver.so. So we cannot use that APIs directly.
 */

#include <array>
#include <map> // for legacy reasons
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include <optional>

#include <android-base/unique_fd.h>
#include <cutils/native_handle.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <utils/String16.h>
#include <utils/Vector.h>
#include <utils/Flattenable.h>

#include "linux/libbinder_driver_exports.h"

using ret_status = int32_t;

namespace android
{

#ifdef BINDER_IPC_32BIT
    using binder_size_t = uint32_t;
    using binder_uintptr_t = uint32_t;
#else
    using binder_size_t = uint64_t;
    using binder_uintptr_t = uint64_t;
#endif

LIBBINDERDRIVER_EXPORTS String8 get_next_pointer_key();

struct AddServiceControlBlock
{
    std::string service_name;
    ::android::sp<::android::RefBase> service_entry;
    bool allowIsolated = false;
    int32_t dumpPriority = 0;
};

LIBBINDERDRIVER_EXPORTS AddServiceControlBlock* make_control_block
    (
    std::string service_name,
    ::android::sp<::android::RefBase> service_entry,
    bool allowIsolated,
    int32_t dumpPriority
    );

LIBBINDERDRIVER_EXPORTS void release_control_block( AddServiceControlBlock* a_cb );

}

