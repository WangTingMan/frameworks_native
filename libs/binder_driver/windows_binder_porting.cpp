#include <linux/binder.h>
#include <linux/binder_internal.h>
#include <linux/binder_internal_control_block_mgr.h>
#include <base/logging.h>

#include <thread>
#include <list>

#include "ipc_connection_token.h"

/**
 * This is the fake implementation of msm-kernel/drivers/android/binder.c
 */

namespace porting_binder {

static int binder_ioctl_write_read(__u32 handle, void* bwr);
static void binder_transaction(binder_proc* proc,
    binder_thread* thread,
    binder_transaction_data* tr, int reply,
    binder_size_t extra_buffers_size);

static inline int copy_from_user(void* to, const void* from,
    unsigned long n)
{
    memcpy(to, from, n);
    return 0;
}

void close_binder(__u32 handle) {
    binder_internal_control_block_mgr::get_instance().shut_down( handle );
}

int32_t fcntl_binder(__u32 handle, uint32_t to_operation, uint32_t parameters) {
    LOG(ERROR) << "No implementation!";
    return 0;
}

int32_t fcntl_binder(__u32 handle, uint32_t to_operation, void* parameters)
{
    int32_t ret = 0;

    switch (to_operation)
    {
    case BINDER_VERSION:
        {
            int* version = reinterpret_cast<int*>(parameters);
            *version = BINDER_CURRENT_PROTOCOL_VERSION;
            LOG(INFO) << "Get binder version.";
            return 0;
        }
        break;
    case BINDER_SET_MAX_THREADS:
        {
            unsigned int* maxThreads = reinterpret_cast<unsigned int*>(parameters);
            *maxThreads = 4;
            LOG(INFO) << "Get binder max threads.";
            return 0;
        }
        break;
    case BINDER_WRITE_READ:
        {
            return binder_ioctl_write_read(handle, parameters);
        }
    case BINDER_ENABLE_ONEWAY_SPAM_DETECTION:
    {
        uint32_t enable;

        if (copy_from_user(&enable, parameters, sizeof(enable))) {
            ret = -EFAULT;
            goto err;
        }

        binder_internal_control_block_mgr::get_instance().set_oneway_spam_detection_enabled((bool)enable);
        return 0;
    }
    case BINDER_SET_CONTEXT_MGR_EXT:
        binder_internal_control_block_mgr::get_instance()
            .set_service_manager( true );
        return 0;
    default:
        break;
    }
    LOG(ERROR) << "No implementation!";

    return 0;

err:
    return ret;
}

__u32 open_binder(const char* a_binder_name, ...)
{
    return binder_internal_control_block_mgr::get_instance().get_fake_fd( a_binder_name );
}

void register_binder_data_handler( std::function<void()> a_fun, bool a_for_aidl )
{
    binder_internal_control_block_mgr::get_instance().set_binder_data_handler( a_fun, a_for_aidl );
}

void debug_invoke()
{
    auto msg = binder_internal_control_block_mgr::get_instance().get_previous_handle_message();
}

int binder_ioctl_write_read(__u32 handle, void* a_bwr)
{
    int ret = 0;
    binder_write_read* wr_cb = reinterpret_cast<binder_write_read*>(a_bwr);
    ret = binder_internal_control_block_mgr::get_instance().handle_write_read_block( handle, wr_cb);

    return ret;
}

void binder_transaction(binder_proc* proc,
    binder_thread* thread,
    binder_transaction_data* tr, int reply,
    binder_size_t extra_buffers_size)
{
    LOG( ERROR ) << "No implementation!";
}

} // namespace porting_binder