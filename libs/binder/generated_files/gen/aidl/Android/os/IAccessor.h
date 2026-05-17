/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IAccessor.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/IAccessor.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IAccessor.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/ParcelFileDescriptor.h>
#include <binder/Status.h>
#include <binder/Trace.h>
#include <cstdint>
#include <utils/String16.h>
#include <utils/StrongPointer.h>
#include <binder/libbinder_export.h>
namespace android {
namespace os {
class LIBBINDER_EXPORTED IAccessorDelegator;

class LIBBINDER_EXPORT IAccessor : public ::android::IInterface {
public:
  typedef IAccessorDelegator DefaultDelegator;
  DECLARE_META_INTERFACE(Accessor)
  enum : int32_t { ERROR_CONNECTION_INFO_NOT_FOUND = 0 };
  enum : int32_t { ERROR_FAILED_TO_CREATE_SOCKET = 1 };
  enum : int32_t { ERROR_FAILED_TO_CONNECT_TO_SOCKET = 2 };
  enum : int32_t { ERROR_FAILED_TO_CONNECT_EACCES = 3 };
  enum : int32_t { ERROR_UNSUPPORTED_SOCKET_FAMILY = 4 };
  virtual ::android::binder::Status addConnection(::android::os::ParcelFileDescriptor* _aidl_return) = 0;
  virtual ::android::binder::Status getInstanceName(::android::String16* _aidl_return) = 0;
};  // class IAccessor

class LIBBINDER_EXPORT IAccessorDefault : public IAccessor {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status addConnection(::android::os::ParcelFileDescriptor* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status getInstanceName(::android::String16* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class IAccessorDefault
}  // namespace os
}  // namespace android
