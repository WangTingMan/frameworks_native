/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IStatsBootstrapAtomService.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/base/core/java/ frameworks/base/core/java/android/os/IStatsBootstrapAtomService.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IStatsBootstrapAtomService.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <android/os/StatsBootstrapAtom.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Status.h>
#include <binder/Trace.h>
#include <utils/StrongPointer.h>

namespace android::os {
class StatsBootstrapAtom;
}  // namespace android::os
namespace android {
namespace os {
class LIBBINDER_EXPORTED IStatsBootstrapAtomServiceDelegator;

class LIBBINDER_EXPORTED IStatsBootstrapAtomService : public ::android::IInterface {
public:
  typedef IStatsBootstrapAtomServiceDelegator DefaultDelegator;
  DECLARE_META_INTERFACE(StatsBootstrapAtomService)
  virtual ::android::binder::Status reportBootstrapAtom(const ::android::os::StatsBootstrapAtom& atom) = 0;
};  // class IStatsBootstrapAtomService

class LIBBINDER_EXPORTED IStatsBootstrapAtomServiceDefault : public IStatsBootstrapAtomService {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status reportBootstrapAtom(const ::android::os::StatsBootstrapAtom& /*atom*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class IStatsBootstrapAtomServiceDefault
}  // namespace os
}  // namespace android
