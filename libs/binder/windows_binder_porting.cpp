#include <linux/binder.h>
#include "linux\binder_internal.h"
#include <linux\binder_internal_control_block.h>
#include <base/logging.h>

#include <thread>
#include <list>

/**
 * This is the fake implementation of msm-kernel/drivers/android/binder.c
 */

namespace porting_binder {

static int binder_ioctl_write_read(__u32 handle, void* bwr);
static int binder_thread_write(__u32 handle,
    binder_uintptr_t binder_buffer, size_t size,
    binder_size_t* consumed);
static int binder_thread_read(__u32 handle,
    binder_uintptr_t binder_buffer, size_t size,
    binder_size_t* consumed, int non_block);
static void binder_transaction(binder_proc* proc,
    binder_thread* thread,
    binder_transaction_data* tr, int reply,
    binder_size_t extra_buffers_size);

template<typename T>
static void get_user(T& value, char* (&a_ptr))
{
    value = *reinterpret_cast<T*>(a_ptr);
    a_ptr += sizeof(T);
}

static inline int copy_from_user(void* to, const void* from,
    unsigned long n)
{
    memcpy(to, from, n);
    return 0;
}

void close_binder(__u32 handle) {
    LOG(ERROR) << "No implementation!";
}

__u32 fcntl_binder(__u32 handle, uint32_t to_operation, uint32_t parameters) {
    LOG(ERROR) << "No implementation!";
    return 0;
}

__u32 fcntl_binder(__u32 handle, uint32_t to_operation, void* parameters)
{
    __u32 ret = 0;
    auto cb = binder_internal_control_block_mgr::get_instance().find_by_id(handle);
    if (!cb)
    {
        return -1;
    }

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
            binder_ioctl_write_read(handle, parameters);
            LOG(INFO) << "write or read data to or from binder driver.";
            return 0;
        }
    case BINDER_ENABLE_ONEWAY_SPAM_DETECTION:
    {
        uint32_t enable;

        if (copy_from_user(&enable, parameters, sizeof(enable))) {
            ret = -EFAULT;
            goto err;
        }

        cb->set_oneway_spam_detection_enabled((bool)enable);
        return 0;
    }
    default:
        break;
    }
    LOG(ERROR) << "No implementation!";

    return 0;

err:
    return ret;
}

__u32 open_binder(const char*, ...)
{
    LOG(ERROR) << "No implementation!";
    auto bc = binder_internal_control_block_mgr::get_instance().create_new_one();
    return bc->get_id();
}

int binder_ioctl_write_read(__u32 handle, void* a_bwr)
{
    int ret = 0;
    struct binder_write_read bwr;

    memcpy(&bwr, a_bwr, sizeof(binder_write_read));

    if (bwr.write_size > 0) {
        ret = binder_thread_write(handle,
            bwr.write_buffer,
            bwr.write_size,
            &bwr.write_consumed);
        if (ret < 0) {
            bwr.read_consumed = 0;
            if (memcpy(a_bwr, &bwr, sizeof(bwr)))
                ret = -EFAULT;
            goto out;
        }
    }
    if (bwr.read_size > 0) {
        ret = binder_thread_read(handle, bwr.read_buffer,
            bwr.read_size,
            &bwr.read_consumed,
            /*filp->f_flags & O_NONBLOCK*/0);

        if (ret < 0) {
            if (memcpy(a_bwr, &bwr, sizeof(bwr)))
                ret = -EFAULT;
            goto out;
        }
    }

    if (memcpy(a_bwr, &bwr, sizeof(bwr))) {
        ret = -EFAULT;
        goto out;
    }
out:
    return ret;
}

int binder_thread_write(__u32 handle,
    binder_uintptr_t binder_buffer, size_t size,
    binder_size_t* consumed)
{
    char* ptr = reinterpret_cast<char*>(binder_buffer) + *consumed;
    uint32_t cmd;
    get_user(cmd, ptr);

    switch (cmd) {
    case BC_INCREFS:
    case BC_ACQUIRE:
    case BC_RELEASE:
        break;
    case BC_TRANSACTION:
    case BC_REPLY:
    {
        binder_transaction_data tr;

        if (copy_from_user(&tr, ptr, sizeof(tr)))
            return -EFAULT;
        ptr += sizeof(tr);
        break;
    }
    default:
        LOG(ERROR) << "No implementation!";
    }

    LOG(ERROR) << "No implementation!";
    return 0;
}

int binder_thread_read(__u32 handle,
    binder_uintptr_t binder_buffer, size_t size,
    binder_size_t* consumed, int non_block)
{
    LOG(ERROR) << "No implementation!";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return 0;
}

void binder_transaction(binder_proc* proc,
    binder_thread* thread,
    binder_transaction_data* tr, int reply,
    binder_size_t extra_buffers_size)
{

}

} // namespace porting_binder