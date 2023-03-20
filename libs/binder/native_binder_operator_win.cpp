#include "native_binder_operator_win.h"

static constexpr uint32_t s_default_buffer_size = 1024;

BinderCommunicationBuffer::BinderCommunicationBuffer()
{
    buffer.resize( s_default_buffer_size );
    valid_size = 0;
}

void BinderCommunicationBuffer::ReallocateSize( uint32_t newSize )
{
    if( newSize < buffer.size() )
    {
        return;
    }
    else
    {
        std::vector<uint8_t> temp_buffer;
        temp_buffer = std::move( buffer );
        buffer.resize( newSize );
        memcpy( buffer.data(), temp_buffer.data(), temp_buffer.size() );
    }
}

void NativeBinderOperatorWin::ReleaseResource()
{
    mOperatorId = -1;
}

