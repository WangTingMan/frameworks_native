/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IServiceManager.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/native/libs/binder/aidl/ frameworks/native/libs/binder/aidl/android/os/IServiceManager.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IServiceManager.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <android/binder_to_string.h>
#include <android/os/ConnectionInfo.h>
#include <android/os/IClientCallback.h>
#include <android/os/IServiceCallback.h>
#include <android/os/IServiceManager.h>
#include <android/os/Service.h>
#include <android/os/ServiceDebugInfo.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <binder/Trace.h>
#include <cstdint>
#include <optional>
#include <string>
#include <tuple>
#include <utils/String16.h>
#include <utils/StrongPointer.h>
#include <vector>

#include <binder/libbinder_export.h>

#ifndef __attribute__
#define __attribute__(...)
#endif

namespace android::os {
class ConnectionInfo;
class IClientCallback;
class IServiceCallback;
class ServiceDebugInfo;
}  // namespace android::os
namespace android {
namespace os {
class LIBBINDER_EXPORTED IServiceManagerDelegator;

class LIBBINDER_EXPORT IServiceManager : public ::android::IInterface {
public:
  typedef IServiceManagerDelegator DefaultDelegator;
  DECLARE_META_INTERFACE(ServiceManager)
  class LIBBINDER_EXPORT CallerContext : public ::android::Parcelable {
  public:
    ::std::string sidName;
    int32_t debugPid = 0;
    int32_t uid = 0;
    inline bool operator==(const CallerContext& _rhs) const {
      return std::tie(sidName, debugPid, uid) == std::tie(_rhs.sidName, _rhs.debugPid, _rhs.uid);
    }
    inline bool operator<(const CallerContext& _rhs) const {
      return std::tie(sidName, debugPid, uid) < std::tie(_rhs.sidName, _rhs.debugPid, _rhs.uid);
    }
    inline bool operator!=(const CallerContext& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const CallerContext& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const CallerContext& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const CallerContext& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.os.IServiceManager.CallerContext");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream _aidl_os;
      _aidl_os << "CallerContext{";
      _aidl_os << "sidName: " << ::android::internal::ToString(sidName);
      _aidl_os << ", debugPid: " << ::android::internal::ToString(debugPid);
      _aidl_os << ", uid: " << ::android::internal::ToString(uid);
      _aidl_os << "}";
      return _aidl_os.str();
    }
  };  // class CallerContext
  enum : int32_t { DUMP_FLAG_PRIORITY_CRITICAL = 1 };
  enum : int32_t { DUMP_FLAG_PRIORITY_HIGH = 2 };
  enum : int32_t { DUMP_FLAG_PRIORITY_NORMAL = 4 };
  enum : int32_t { DUMP_FLAG_PRIORITY_DEFAULT = 8 };
  enum : int32_t { DUMP_FLAG_PRIORITY_ALL = 15 };
  enum : int32_t { FLAG_IS_LAZY_SERVICE = 1073741824 };
  enum : int32_t { DUMP_FLAG_PROTO = 16 };
  virtual ::android::binder::Status getService(const ::std::string& name, ::android::sp<::android::IBinder>* _aidl_return) __attribute__((deprecated("TODO(b/355394904): Use getService2 instead. This does not return metadata that is included in ServiceWithMetadata"))) = 0;
  virtual ::android::binder::Status getService2(const ::std::string& name, ::android::os::Service* _aidl_return) = 0;
  virtual ::android::binder::Status checkService(const ::std::string& name, ::android::sp<::android::IBinder>* _aidl_return) __attribute__((deprecated("TODO(b/355394904): Use checkService2 instead. This does not return metadata that is included in ServiceWithMetadata"))) = 0;
  virtual ::android::binder::Status checkService2(const ::std::string& name, ::android::os::Service* _aidl_return) = 0;
  virtual ::android::binder::Status addService(const ::std::string& name, const ::android::sp<::android::IBinder>& service, bool allowIsolated, int32_t dumpPriority) = 0;
  virtual ::android::binder::Status listServices(int32_t dumpPriority, ::std::vector<::std::string>* _aidl_return) = 0;
  virtual ::android::binder::Status registerForNotifications(const ::std::string& name, const ::android::sp<::android::os::IServiceCallback>& callback) = 0;
  virtual ::android::binder::Status unregisterForNotifications(const ::std::string& name, const ::android::sp<::android::os::IServiceCallback>& callback) = 0;
  virtual ::android::binder::Status isDeclared(const ::std::string& name, bool* _aidl_return) = 0;
  virtual ::android::binder::Status getDeclaredInstances(const ::std::string& iface, ::std::vector<::std::string>* _aidl_return) = 0;
  virtual ::android::binder::Status updatableViaApex(const ::std::string& name, ::std::optional<::std::string>* _aidl_return) = 0;
  virtual ::android::binder::Status getUpdatableNames(const ::std::string& apexName, ::std::vector<::std::string>* _aidl_return) = 0;
  virtual ::android::binder::Status getConnectionInfo(const ::std::string& name, ::std::optional<::android::os::ConnectionInfo>* _aidl_return) = 0;
  virtual ::android::binder::Status registerClientCallback(const ::std::string& name, const ::android::sp<::android::IBinder>& service, const ::android::sp<::android::os::IClientCallback>& callback) = 0;
  virtual ::android::binder::Status tryUnregisterService(const ::std::string& name, const ::android::sp<::android::IBinder>& service) = 0;
  virtual ::android::binder::Status getServiceDebugInfo(::std::vector<::android::os::ServiceDebugInfo>* _aidl_return) = 0;
  virtual ::android::binder::Status checkServiceAccess(const ::android::os::IServiceManager::CallerContext& callerCtx, const ::std::string& name, const ::std::string& permission, bool* _aidl_return) = 0;
};  // class IServiceManager

class LIBBINDER_EXPORT IServiceManagerDefault : public IServiceManager {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status getService(const ::std::string& /*name*/, ::android::sp<::android::IBinder>* /*_aidl_return*/) override __attribute__((deprecated("TODO(b/355394904): Use getService2 instead. This does not return metadata that is included in ServiceWithMetadata"))) {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status getService2(const ::std::string& /*name*/, ::android::os::Service* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status checkService(const ::std::string& /*name*/, ::android::sp<::android::IBinder>* /*_aidl_return*/) override __attribute__((deprecated("TODO(b/355394904): Use checkService2 instead. This does not return metadata that is included in ServiceWithMetadata"))) {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status checkService2(const ::std::string& /*name*/, ::android::os::Service* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status addService(const ::std::string& /*name*/, const ::android::sp<::android::IBinder>& /*service*/, bool /*allowIsolated*/, int32_t /*dumpPriority*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status listServices(int32_t /*dumpPriority*/, ::std::vector<::std::string>* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status registerForNotifications(const ::std::string& /*name*/, const ::android::sp<::android::os::IServiceCallback>& /*callback*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status unregisterForNotifications(const ::std::string& /*name*/, const ::android::sp<::android::os::IServiceCallback>& /*callback*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status isDeclared(const ::std::string& /*name*/, bool* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status getDeclaredInstances(const ::std::string& /*iface*/, ::std::vector<::std::string>* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status updatableViaApex(const ::std::string& /*name*/, ::std::optional<::std::string>* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status getUpdatableNames(const ::std::string& /*apexName*/, ::std::vector<::std::string>* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status getConnectionInfo(const ::std::string& /*name*/, ::std::optional<::android::os::ConnectionInfo>* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status registerClientCallback(const ::std::string& /*name*/, const ::android::sp<::android::IBinder>& /*service*/, const ::android::sp<::android::os::IClientCallback>& /*callback*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status tryUnregisterService(const ::std::string& /*name*/, const ::android::sp<::android::IBinder>& /*service*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status getServiceDebugInfo(::std::vector<::android::os::ServiceDebugInfo>* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
  ::android::binder::Status checkServiceAccess(const ::android::os::IServiceManager::CallerContext& /*callerCtx*/, const ::std::string& /*name*/, const ::std::string& /*permission*/, bool* /*_aidl_return*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class IServiceManagerDefault
}  // namespace os
}  // namespace android

#ifdef __attribute__
#undef __attribute__
#endif
