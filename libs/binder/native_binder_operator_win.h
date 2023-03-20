#pragma once

#include <atomic>
#include <vector>

class BinderCommunicationBuffer
{
public:

    BinderCommunicationBuffer();

    std::vector<uint8_t>& GetBuffer()
    {
        return buffer;
    }

    uint32_t GetValidSize()
    {
        return valid_size;
    }

    void ReallocateSize( uint32_t newSize );

private:

    std::vector<uint8_t> buffer;
    uint32_t valid_size = 0;
};

class NativeBinderOperatorWin
{

public:

    /**
     * Release all the hold resource related with mOperatorId
     */
    void ReleaseResource();

    bool IsValid() const
    {
        return mOperatorId != -1;
    }

    int32_t GetId()const
    {
        return mOperatorId;
    }

private:

    int32_t mOperatorId = -1;
    BinderCommunicationBuffer mReceivedBuffer;
    BinderCommunicationBuffer mSendBuffer;
};

