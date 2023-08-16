#include "android.h"
#include <cstdlib>
#include <memory>
#include <string>

void freecon(char* con)
{
    free(con);
}

int getpidcon(pid_t pid, char** con)
{
    std::string fake_name("pid_con");
    int alloc_size = sizeof(char) * fake_name.size() + 2;
    char* ptr = reinterpret_cast<char*>( malloc(alloc_size) );
    if (ptr)
    {
        memset(ptr, 0x00, alloc_size);
        strncpy_s(ptr, alloc_size, fake_name.c_str(), fake_name.size());
    }

    *con = ptr;
    return 0;
}

int getcon(char** con)
{
    getpidcon(0, con);
    return 0;
}

int selinux_status_updated(void)
{
    return 0;
}

void selabel_close(struct selabel_handle* handle)
{
    delete handle;
}

struct selabel_handle* selinux_android_vendor_service_context_handle(void)
{
    selabel_handle* handle_ = new selabel_handle;
    return handle_;
}

struct selabel_handle* selinux_android_service_context_handle(void)
{
    selabel_handle* handle_ = new selabel_handle;
    return handle_;
}

void selinux_set_callback(int type, union selinux_callback cb)
{

}

int selinux_vendor_log_callback(int type, const char* fmt, ...)
{
    return 0;
}

int selinux_log_callback(int type, const char* fmt, ...)
{
    return 0;
}

int selinux_status_open(int fallback)
{
    return 0;
}

int selinux_check_access(const char* scon, const char* tcon, const char* class_, const char* perm, void* aux)
{
    return 0;
}

int selabel_lookup(struct selabel_handle* rec, char** con,
    const char* key, int type)
{
    return 0;
}