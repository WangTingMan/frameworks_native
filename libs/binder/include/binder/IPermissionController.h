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

#pragma once

#ifndef __ANDROID_VNDK__

#include <binder/Common.h>
#include <binder/IInterface.h>
#include <stdlib.h>

#include <binder/libbinder_export.h>

#ifndef uid_t
#define uid_t int32_t
#endif

namespace android {

// ----------------------------------------------------------------------

<<<<<<< HEAD
class LIBBINDER_EXPORT IPermissionController : public IInterface
{
=======
class LIBBINDER_EXPORTED IPermissionController : public IInterface {
>>>>>>> d3fb93fb73
public:
    DECLARE_META_INTERFACE(PermissionController)

    virtual bool checkPermission(const String16& permission, int32_t pid, int32_t uid) = 0;

    virtual int32_t noteOp(const String16& op, int32_t uid, const String16& packageName) = 0;

    virtual void getPackagesForUid(const uid_t uid, Vector<String16> &packages) = 0;

    virtual bool isRuntimePermission(const String16& permission) = 0;

    virtual int getPackageUid(const String16& package, int flags) = 0;

    enum {
        CHECK_PERMISSION_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION,
        NOTE_OP_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION + 1,
        GET_PACKAGES_FOR_UID_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION + 2,
        IS_RUNTIME_PERMISSION_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION + 3,
        GET_PACKAGE_UID_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION + 4
    };
};

// ----------------------------------------------------------------------

<<<<<<< HEAD
class LIBBINDER_EXPORT BnPermissionController : public BnInterface<IPermissionController>
{
=======
class LIBBINDER_EXPORTED BnPermissionController : public BnInterface<IPermissionController> {
>>>>>>> d3fb93fb73
public:
    // NOLINTNEXTLINE(google-default-arguments)
    virtual status_t    onTransact( uint32_t code,
                                    const Parcel& data,
                                    Parcel* reply,
                                    uint32_t flags = 0);
};

// ----------------------------------------------------------------------

} // namespace android

#else // __ANDROID_VNDK__
#error "This header is not visible to vendors"
#endif // __ANDROID_VNDK__
