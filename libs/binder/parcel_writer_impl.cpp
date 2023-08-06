#include <binder/parcel_writer_impl.h>

namespace android
{

void register_detail_parcel_writer()
{
    parcel_writer_maker maker;
    maker = []()
    {
        return std::make_shared<parcel_writer_impl>();
    };

    register_parcel_writer_maker( maker );
}

void parcel_writer_impl::copy_to( uint8_t* a_dest, uint32_t a_size )
{
    memcpy( a_dest, m_detail.data(), a_size );
}

uint32_t parcel_writer_impl::size()
{
    return m_detail.dataSize();
}


}
