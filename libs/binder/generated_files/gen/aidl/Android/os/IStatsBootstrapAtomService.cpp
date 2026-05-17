/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IStatsBootstrapAtomService.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/base/core/java/ frameworks/base/core/java/android/os/IStatsBootstrapAtomService.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/IStatsBootstrapAtomService.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#include <android/os/IStatsBootstrapAtomService.h>
#include <android/os/BpStatsBootstrapAtomService.h>
namespace android {
namespace os {
DO_NOT_DIRECTLY_USE_ME_IMPLEMENT_META_INTERFACE(StatsBootstrapAtomService, "android.os.IStatsBootstrapAtomService")
}  // namespace os
}  // namespace android
#include <android/os/BpStatsBootstrapAtomService.h>
#include <android/os/BnStatsBootstrapAtomService.h>
#include <binder/Parcel.h>

namespace android {
namespace os {

BpStatsBootstrapAtomService::BpStatsBootstrapAtomService(const ::android::sp<::android::IBinder>& _aidl_impl)
    : BpInterface<IStatsBootstrapAtomService>(_aidl_impl){
}

::android::binder::Status BpStatsBootstrapAtomService::reportBootstrapAtom(const ::android::os::StatsBootstrapAtom& atom) {
  ::android::Parcel _aidl_data;
  _aidl_data.markForBinder(remoteStrong());
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::binder::Status _aidl_status;
  ::android::binder::ScopedTrace _aidl_trace(ATRACE_TAG_AIDL, "AIDL::cpp::IStatsBootstrapAtomService::reportBootstrapAtom::cppClient");
  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeParcelable(atom);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = remote()->transact(BnStatsBootstrapAtomService::TRANSACTION_reportBootstrapAtom, _aidl_data, nullptr, ::android::IBinder::FLAG_ONEWAY);
  if (_aidl_ret_status == ::android::UNKNOWN_TRANSACTION && IStatsBootstrapAtomService::getDefaultImpl()) [[unlikely]] {
     return IStatsBootstrapAtomService::getDefaultImpl()->reportBootstrapAtom(atom);
  }
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_error:
  _aidl_status.setFromStatusT(_aidl_ret_status);
  return _aidl_status;
}

}  // namespace os
}  // namespace android
#include <android/os/BnStatsBootstrapAtomService.h>
#include <binder/Parcel.h>
#include <binder/Stability.h>

namespace android {
namespace os {

BnStatsBootstrapAtomService::BnStatsBootstrapAtomService()
{
  ::android::internal::Stability::markCompilationUnit(this);
}

::android::status_t BnStatsBootstrapAtomService::onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  switch (_aidl_code) {
  case BnStatsBootstrapAtomService::TRANSACTION_reportBootstrapAtom:
  {
    ::android::os::StatsBootstrapAtom in_atom;
    if (!(_aidl_data.checkInterface(this))) {
      _aidl_ret_status = ::android::BAD_TYPE;
      break;
    }
    ::android::binder::ScopedTrace _aidl_trace(ATRACE_TAG_AIDL, "AIDL::cpp::IStatsBootstrapAtomService::reportBootstrapAtom::cppServer");
    _aidl_ret_status = _aidl_data.readParcelable(&in_atom);
    if (((_aidl_ret_status) != (::android::OK))) {
      break;
    }
    if (auto st = _aidl_data.enforceNoDataAvail(); !st.isOk()) {
      _aidl_ret_status = st.writeToParcel(_aidl_reply);
      break;
    }
    ::android::binder::Status _aidl_status(reportBootstrapAtom(in_atom));
  }
  break;
  default:
  {
    _aidl_ret_status = ::android::BBinder::onTransact(_aidl_code, _aidl_data, _aidl_reply, _aidl_flags);
  }
  break;
  }
  if (_aidl_ret_status == ::android::UNEXPECTED_NULL) {
    _aidl_ret_status = ::android::binder::Status::fromExceptionCode(::android::binder::Status::EX_NULL_POINTER).writeOverParcel(_aidl_reply);
  }
  return _aidl_ret_status;
}

}  // namespace os
}  // namespace android
