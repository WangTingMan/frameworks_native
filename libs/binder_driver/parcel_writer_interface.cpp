#include "parcel_writer_interface.h"

#include <shared_mutex>

namespace
{
    std::shared_mutex s_writer_mutex;
    android::parcel_writer_maker s_maker;
    android::parcel_writer_maker s_hidl_parcel_maker;
}

void android::register_parcel_writer_maker( parcel_writer_maker a_maker )
{
    std::lock_guard<std::shared_mutex> locker( s_writer_mutex );
    s_maker = a_maker;
}

void android::register_hidl_parcel_writer_maker( parcel_writer_maker a_maker )
{
    std::lock_guard<std::shared_mutex> locker( s_writer_mutex );
    s_hidl_parcel_maker = a_maker;
}

android::parcel_writer_maker android::get_parcel_writer_maker()
{
    std::shared_lock<std::shared_mutex> lcker( s_writer_mutex );
    if( !s_maker )
    {
        ALOGE( "empty maker!" );
    }

    return s_maker;
}

android::parcel_writer_maker android::get_hidl_parcel_writer_maker()
{
    std::shared_lock<std::shared_mutex> lcker( s_writer_mutex );
    if( !s_hidl_parcel_maker )
    {
        ALOGE( "empty maker!" );
    }

    return s_hidl_parcel_maker;
}
