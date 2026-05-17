/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IServiceCallback.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/IServiceCallback.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IServiceCallback.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <utils/Errors.h>
#include <android/os/IServiceCallback.h>
#include <binder/libbinder_export.h>
namespace android {
namespace os {
class LIBBINDER_EXPORT BpServiceCallback : public ::android::BpInterface<IServiceCallback> {
public:
  explicit BpServiceCallback(const ::android::sp<::android::IBinder>& _aidl_impl);
  virtual ~BpServiceCallback() = default;
  ::android::binder::Status onRegistration(const ::std::string& name, const ::android::sp<::android::IBinder>& binder) override;
};  // class BpServiceCallback
}  // namespace os
}  // namespace android
