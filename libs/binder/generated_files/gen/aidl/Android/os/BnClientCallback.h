/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IClientCallback.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/IClientCallback.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IClientCallback.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IInterface.h>
#include <android/os/IClientCallback.h>
#include <android/os/BnClientCallback.h>
#include <binder/Delegate.h>
#include <binder/libbinder_export.h>

namespace android {
namespace os {
class LIBBINDER_EXPORT BnClientCallback : public ::android::BnInterface<IClientCallback> {
public:
  static constexpr uint32_t TRANSACTION_onClients = ::android::IBinder::FIRST_CALL_TRANSACTION + 0;
  explicit BnClientCallback();
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) override;
};  // class BnClientCallback

class LIBBINDER_EXPORT IClientCallbackDelegator : public BnClientCallback {
public:
  explicit IClientCallbackDelegator(const ::android::sp<IClientCallback> &impl) : _aidl_delegate(impl) {}

  ::android::sp<IClientCallback> getImpl() { return _aidl_delegate; }
  ::android::binder::Status onClients(const ::android::sp<::android::IBinder>& registered, bool hasClients) override {
    return _aidl_delegate->onClients(registered, hasClients);
  }
private:
  ::android::sp<IClientCallback> _aidl_delegate;
};  // class IClientCallbackDelegator
}  // namespace os
}  // namespace android
