/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/7b5f55ceac8667b805b9fc05527c05040d45a287/out/android/os/IAccessor.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/IAccessor.aidl out/soong/.temp/sbox/7b5f55ceac8667b805b9fc05527c05040d45a287/out out/soong/.temp/sbox/7b5f55ceac8667b805b9fc05527c05040d45a287/out/android/os/IAccessor.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/Errors.h>
#include <android/os/IAccessor.h>

namespace android {
namespace os {
class LIBBINDER_EXPORTED BpAccessor : public ::android::BpInterface<IAccessor> {
public:
  explicit BpAccessor(const ::android::sp<::android::IBinder>& _aidl_impl);
  virtual ~BpAccessor() = default;
  ::android::binder::Status addConnection(::android::os::ParcelFileDescriptor* _aidl_return) override;
  ::android::binder::Status getInstanceName(::android::String16* _aidl_return) override;
};  // class BpAccessor
}  // namespace os
}  // namespace android
