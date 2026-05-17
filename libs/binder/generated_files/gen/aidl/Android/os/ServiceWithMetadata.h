/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/ServiceWithMetadata.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/ServiceWithMetadata.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/ServiceWithMetadata.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <android/binder_to_string.h>
#include <binder/IBinder.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <tuple>
#include <utils/String16.h>
#include <binder/libbinder_export.h>
namespace android {
namespace os {
class LIBBINDER_EXPORT ServiceWithMetadata : public ::android::Parcelable {
public:
  ::android::sp<::android::IBinder> service;
  bool isLazyService = false;
  inline bool operator==(const ServiceWithMetadata& _rhs) const {
    return std::tie(service, isLazyService) == std::tie(_rhs.service, _rhs.isLazyService);
  }
  inline bool operator<(const ServiceWithMetadata& _rhs) const {
    return std::tie(service, isLazyService) < std::tie(_rhs.service, _rhs.isLazyService);
  }
  inline bool operator!=(const ServiceWithMetadata& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const ServiceWithMetadata& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const ServiceWithMetadata& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const ServiceWithMetadata& _rhs) const {
    return !(_rhs < *this);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.os.ServiceWithMetadata");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "ServiceWithMetadata{";
    _aidl_os << "service: " << ::android::internal::ToString(service);
    _aidl_os << ", isLazyService: " << ::android::internal::ToString(isLazyService);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};  // class ServiceWithMetadata
}  // namespace os
}  // namespace android
