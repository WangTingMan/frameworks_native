/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IClientCallback.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/IClientCallback.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IClientCallback.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Status.h>
#include <binder/Trace.h>
#include <utils/StrongPointer.h>
#include <binder/libbinder_export.h>
namespace android {
namespace os {
class LIBBINDER_EXPORTED IClientCallbackDelegator;

class LIBBINDER_EXPORT IClientCallback : public ::android::IInterface {
public:
  typedef IClientCallbackDelegator DefaultDelegator;
  DECLARE_META_INTERFACE(ClientCallback)
  virtual ::android::binder::Status onClients(const ::android::sp<::android::IBinder>& registered, bool hasClients) = 0;
};  // class IClientCallback

class LIBBINDER_EXPORT IClientCallbackDefault : public IClientCallback {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status onClients(const ::android::sp<::android::IBinder>& /*registered*/, bool /*hasClients*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class IClientCallbackDefault
}  // namespace os
}  // namespace android
