/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IStatsBootstrapAtomService.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/base/core/java/ frameworks/base/core/java/android/os/IStatsBootstrapAtomService.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IStatsBootstrapAtomService.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <binder/IInterface.h>
#include <android/os/IStatsBootstrapAtomService.h>
#include <android/os/BnStatsBootstrapAtomService.h>
#include <binder/Delegate.h>


namespace android {
namespace os {
class LIBBINDER_EXPORTED BnStatsBootstrapAtomService : public ::android::BnInterface<IStatsBootstrapAtomService> {
public:
  static constexpr uint32_t TRANSACTION_reportBootstrapAtom = ::android::IBinder::FIRST_CALL_TRANSACTION + 0;
  explicit BnStatsBootstrapAtomService();
  ::android::status_t onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) override;
};  // class BnStatsBootstrapAtomService

class LIBBINDER_EXPORTED IStatsBootstrapAtomServiceDelegator : public BnStatsBootstrapAtomService {
public:
  explicit IStatsBootstrapAtomServiceDelegator(const ::android::sp<IStatsBootstrapAtomService> &impl) : _aidl_delegate(impl) {}

  ::android::sp<IStatsBootstrapAtomService> getImpl() { return _aidl_delegate; }
  ::android::binder::Status reportBootstrapAtom(const ::android::os::StatsBootstrapAtom& atom) override {
    return _aidl_delegate->reportBootstrapAtom(atom);
  }
private:
  ::android::sp<IStatsBootstrapAtomService> _aidl_delegate;
};  // class IStatsBootstrapAtomServiceDelegator
}  // namespace os
}  // namespace android
