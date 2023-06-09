/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>

#include <binder/IMemory.h>
#include <binder/MemoryHeapBase.h>

#include <binder/libbinder_export.h>

namespace android {
// ----------------------------------------------------------------------------

class SimpleBestFitAllocator;

// ----------------------------------------------------------------------------

class LIBBINDER_EXPORT MemoryDealer : public RefBase
{
public:
    explicit MemoryDealer(size_t size, const char* name = nullptr,
            uint32_t flags = 0 /* or bits such as MemoryHeapBase::READ_ONLY */ );

    virtual sp<IMemory> allocate(size_t size);
    virtual void        dump(const char* what) const;

    // allocations are aligned to some value. return that value so clients can account for it.
    static size_t      getAllocationAlignment();

    sp<IMemoryHeap> getMemoryHeap() const { return heap(); }

protected:
    virtual ~MemoryDealer();

private:
    friend class Allocation;
    virtual void                deallocate(size_t offset);
    const sp<IMemoryHeap>&      heap() const;
    SimpleBestFitAllocator*     allocator() const;

    sp<IMemoryHeap>             mHeap;
    SimpleBestFitAllocator*     mAllocator;
};


// ----------------------------------------------------------------------------
} // namespace android
