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
  inline bool operator!=(const ServiceDebugInfo& rhs) const {
    return std::tie(name, debugPid) != std::tie(rhs.name, rhs.debugPid);
  }
  inline bool operator<(const ServiceDebugInfo& rhs) const {
    return std::tie(name, debugPid) < std::tie(rhs.name, rhs.debugPid);
  }
  inline bool operator<=(const ServiceDebugInfo& rhs) const {
    return std::tie(name, debugPid) <= std::tie(rhs.name, rhs.debugPid);
  }
  inline bool operator==(const ServiceDebugInfo& rhs) const {
    return std::tie(name, debugPid) == std::tie(rhs.name, rhs.debugPid);
  }
  inline bool operator>(const ServiceDebugInfo& rhs) const {
    return std::tie(name, debugPid) > std::tie(rhs.name, rhs.debugPid);
  }
  inline bool operator>=(const ServiceDebugInfo& rhs) const {
    return std::tie(name, debugPid) >= std::tie(rhs.name, rhs.debugPid);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    static const ::android::StaticString16 DESCIPTOR (u"android.os.ServiceDebugInfo");
    return DESCIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream os;
    os << "ServiceDebugInfo{";
    os << "name: " << ::android::internal::ToString(name);
    os << ", debugPid: " << ::android::internal::ToString(debugPid);
    os << "}";
    return os.str();
  }
};  // class ServiceDebugInfo
}  // namespace os
}  // namespace android
