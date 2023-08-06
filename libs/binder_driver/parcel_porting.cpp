#include "parcel_porting.h"

#define BINDER_WITH_KERNEL_IPC

// This macro should never be used at runtime, as a too large value
// of s could cause an integer overflow. Instead, you should always
// use the wrapper function pad_size()
#define PAD_SIZE_UNSAFE(s) (((s) + 3) & ~3UL)

static size_t pad_size( size_t s )
{
    if( s > ( std::numeric_limits<size_t>::max() - 3 ) )
    {
        LOG_ALWAYS_FATAL( "pad size too big %zu", s );
    }
    return PAD_SIZE_UNSAFE( s );
}

namespace android
{

String8 get_next_pointer_key()
{
    String8 key;
    key.setTo( "next_is_a_pointer" );
    return key;
}

AddServiceControlBlock* make_control_block
    (
    std::string service_name,
    ::android::sp<::android::RefBase> service_entry,
    bool allowIsolated,
    int32_t dumpPriority
    )
{
    AddServiceControlBlock* cb = nullptr;
    cb = new AddServiceControlBlock;

    cb->service_name = service_name;
    cb->service_entry = service_entry;
    cb->allowIsolated = allowIsolated;
    cb->dumpPriority = dumpPriority;

    return cb;
}

void release_control_block( AddServiceControlBlock* a_cb )
{
    delete a_cb;
}

}

