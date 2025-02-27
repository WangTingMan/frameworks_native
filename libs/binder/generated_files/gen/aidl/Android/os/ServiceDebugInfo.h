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
    static const ::android::StaticString16 DESCRIPTOR (u"android.os.ServiceDebugInfo");
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
