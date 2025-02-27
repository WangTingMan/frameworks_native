/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/7b5f55ceac8667b805b9fc05527c05040d45a287/out/android/os/IAccessor.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/IAccessor.aidl out/soong/.temp/sbox/7b5f55ceac8667b805b9fc05527c05040d45a287/out out/soong/.temp/sbox/7b5f55ceac8667b805b9fc05527c05040d45a287/out/android/os/IAccessor.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IInterface.h>
#include <android/os/IAccessor.h>
#include <android/os/BnAccessor.h>
#include <binder/Delegate.h>


namespace android {
namespace os {
class LIBBINDER_EXPORTED BnAccessor : public ::android::BnInterface<IAccessor> {
public:
  static constexpr uint32_t TRANSACTION_addConnection = ::android::IBinder::FIRST_CALL_TRANSACTION + 0;
  static constexpr uint32_t TRANSACTION_getInstanceName = ::android::IBinder::FIRST_CALL_TRANSACTION + 1;
  explicit BnAccessor();
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) override;
};  // class BnAccessor

class LIBBINDER_EXPORTED IAccessorDelegator : public BnAccessor {
public:
  explicit IAccessorDelegator(const ::android::sp<IAccessor> &impl) : _aidl_delegate(impl) {}

  ::android::sp<IAccessor> getImpl() { return _aidl_delegate; }
  ::android::binder::Status addConnection(::android::os::ParcelFileDescriptor* _aidl_return) override {
    return _aidl_delegate->addConnection(_aidl_return);
  }
  ::android::binder::Status getInstanceName(::android::String16* _aidl_return) override {
    return _aidl_delegate->getInstanceName(_aidl_return);
  }
private:
  ::android::sp<IAccessor> _aidl_delegate;
};  // class IAccessorDelegator
}  // namespace os
}  // namespace android
