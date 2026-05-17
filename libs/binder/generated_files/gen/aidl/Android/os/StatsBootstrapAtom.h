/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/StatsBootstrapAtom.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/base/core/java/ frameworks/base/core/java/android/os/StatsBootstrapAtom.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/StatsBootstrapAtom.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <android/binder_to_string.h>
#include <android/os/StatsBootstrapAtomValue.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <cstdint>
#include <tuple>
#include <utils/String16.h>
#include <vector>

namespace android::os {
class StatsBootstrapAtomValue;
}  // namespace android::os
namespace android {
namespace os {
class LIBBINDER_EXPORTED StatsBootstrapAtom : public ::android::Parcelable {
public:
  int32_t atomId = 0;
  ::std::vector<::android::os::StatsBootstrapAtomValue> values;
  inline bool operator==(const StatsBootstrapAtom& _rhs) const {
    return std::tie(atomId, values) == std::tie(_rhs.atomId, _rhs.values);
  }
  inline bool operator<(const StatsBootstrapAtom& _rhs) const {
    return std::tie(atomId, values) < std::tie(_rhs.atomId, _rhs.values);
  }
  inline bool operator!=(const StatsBootstrapAtom& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const StatsBootstrapAtom& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const StatsBootstrapAtom& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const StatsBootstrapAtom& _rhs) const {
    return !(_rhs < *this);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.os.StatsBootstrapAtom");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "StatsBootstrapAtom{";
    _aidl_os << "atomId: " << ::android::internal::ToString(atomId);
    _aidl_os << ", values: " << ::android::internal::ToString(values);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};  // class StatsBootstrapAtom
}  // namespace os
}  // namespace android
