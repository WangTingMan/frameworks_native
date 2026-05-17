/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IServiceCallback.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/IServiceCallback.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IServiceCallback.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IInterface.h>
#include <android/os/IServiceCallback.h>
#include <android/os/BnServiceCallback.h>
#include <binder/Delegate.h>
#include <binder/libbinder_export.h>

namespace android {
namespace os {
class LIBBINDER_EXPORT BnServiceCallback : public ::android::BnInterface<IServiceCallback> {
public:
  static constexpr uint32_t TRANSACTION_onRegistration = ::android::IBinder::FIRST_CALL_TRANSACTION + 0;
  explicit BnServiceCallback();
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) override;
};  // class BnServiceCallback

class LIBBINDER_EXPORT IServiceCallbackDelegator : public BnServiceCallback {
public:
  explicit IServiceCallbackDelegator(const ::android::sp<IServiceCallback> &impl) : _aidl_delegate(impl) {}

  ::android::sp<IServiceCallback> getImpl() { return _aidl_delegate; }
  ::android::binder::Status onRegistration(const ::std::string& name, const ::android::sp<::android::IBinder>& binder) override {
    return _aidl_delegate->onRegistration(name, binder);
  }
private:
  ::android::sp<IServiceCallback> _aidl_delegate;
};  // class IServiceCallbackDelegator
}  // namespace os
}  // namespace android
