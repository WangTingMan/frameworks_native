/*
 * Copyright (C) 2005 The Android Open Source Project
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

#define LOG_TAG "IInterface"
#include <binder/IInterface.h>

#ifdef _MSC_VER
#include <binder_driver/ipc_connection_token.h>
#endif

namespace android {

// ---------------------------------------------------------------------------

IInterface::IInterface() 
    : RefBase() {
}

IInterface::~IInterface() {
}

// static
sp<IBinder> IInterface::asBinder(const IInterface* iface)
{
    if (iface == nullptr) return nullptr;
    return sp<IBinder>::fromExisting(const_cast<IInterface*>(iface)->onAsBinder());
}

// static
sp<IBinder> IInterface::asBinder(const sp<IInterface>& iface)
{
    if (iface == nullptr) return nullptr;
#ifdef _MSC_VER
    sp<IBinder> ret = sp<IBinder>::fromExisting(iface->onAsBinder());
    String16 descriptor = ret->getInterfaceDescriptor();
    ipc_connection_token_mgr::get_instance().add_local_service( String8(descriptor).c_str(), ret);
    return ret;
#else
    return sp<IBinder>::fromExisting( iface->onAsBinder() );
#endif
}


// ---------------------------------------------------------------------------

} // namespace android
