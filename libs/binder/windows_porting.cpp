#include "binder\windows_porting.h"
#include "base\process\process.h"

#include <windows.h>
#include <algorithm>

uint32_t getpagesize()
{
    static int pagesize = 0;
    if( pagesize == 0 )
    {
        SYSTEM_INFO system_info;
        GetSystemInfo( &system_info );
        pagesize = std::max( system_info.dwPageSize,
            system_info.dwAllocationGranularity );
    }
    return pagesize;
}

uint32_t getuid()
{
    return base::Process::Current().Pid();
}

