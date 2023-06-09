#pragma once

#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Status.h>
#include <string>
#include <utils/StrongPointer.h>

namespace android {
namespace os {
class IServiceCallback : public ::android::IInterface {
public:
  DECLARE_META_INTERFACE(ServiceCallback)
  virtual ::android::binder::Status onRegistration(const ::std::string& name, const ::android::sp<::android::IBinder>& binder) = 0;
};  // class IServiceCallback

class IServiceCallbackDefault : public IServiceCallback {
public:
  ::android::IBinder* onAsBinder() override {
    return nullptr;
  }
  ::android::binder::Status onRegistration(const ::std::string& /*name*/, const ::android::sp<::android::IBinder>& /*binder*/) override {
    return ::android::binder::Status::fromStatusT(::android::UNKNOWN_TRANSACTION);
  }
};  // class IServiceCallbackDefault
}  // namespace os
}  // namespace android
