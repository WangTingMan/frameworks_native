/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/ServiceDebugInfo.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/ServiceDebugInfo.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/ServiceDebugInfo.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <android/binder_to_string.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <cstdint>
#include <string>
#include <tuple>
#include <utils/String16.h>
#include <binder/libbinder_export.h>
namespace android {
namespace os {
class LIBBINDER_EXPORT ServiceDebugInfo : public ::android::Parcelable {
public:
  ::std::string name;
  int32_t debugPid = 0;
  inline bool operator==(const ServiceDebugInfo& _rhs) const {
    return std::tie(name, debugPid) == std::tie(_rhs.name, _rhs.debugPid);
  }
  inline bool operator<(const ServiceDebugInfo& _rhs) const {
    return std::tie(name, debugPid) < std::tie(_rhs.name, _rhs.debugPid);
  }
  inline bool operator!=(const ServiceDebugInfo& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const ServiceDebugInfo& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const ServiceDebugInfo& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const ServiceDebugInfo& _rhs) const {
    return !(_rhs < *this);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.os.ServiceDebugInfo");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "ServiceDebugInfo{";
    _aidl_os << "name: " << ::android::internal::ToString(name);
    _aidl_os << ", debugPid: " << ::android::internal::ToString(debugPid);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};  // class ServiceDebugInfo
}  // namespace os
}  // namespace android
