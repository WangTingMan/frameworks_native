#include <linux/binder.h>
#include <base/logging.h>

namespace porting_binder {

void close_binder(__u32 handle) {
    LOG(ERROR) << "No implementation!";
}

__u32 fcntl_binder(__u32 handle, uint32_t to_operation, uint32_t parameters) {
    LOG(ERROR) << "No implementation!";
    return handle;
}

} // namespace porting_binder