#include <android/os/IServiceCallback.h>
#include <android/os/BpServiceCallback.h>
namespace android {
namespace os {
DO_NOT_DIRECTLY_USE_ME_IMPLEMENT_META_INTERFACE(ServiceCallback, "android.os.IServiceCallback")
}  // namespace os
}  // namespace android
#include <android/os/BpServiceCallback.h>
#include <android/os/BnServiceCallback.h>
#include <binder/Parcel.h>
#include <android-base/macros.h>

#ifdef _MSC_VER
#include <binder_driver/ipc_connection_token.h>
#include <binder/ProcessState.h>
#endif

namespace android {
namespace os {

BpServiceCallback::BpServiceCallback(const ::android::sp<::android::IBinder>& _aidl_impl)
    : BpInterface<IServiceCallback>(_aidl_impl){
}

::android::binder::Status BpServiceCallback::onRegistration(const ::std::string& name, const ::android::sp<::android::IBinder>& binder) {
  ::android::Parcel _aidl_data;
  _aidl_data.markForBinder(remoteStrong());
  ::android::Parcel _aidl_reply;
  ::android::status_t _aidl_ret_status = ::android::OK;
  ::android::binder::Status _aidl_status;
  _aidl_ret_status = _aidl_data.writeInterfaceToken(getInterfaceDescriptor());
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
  _aidl_ret_status = _aidl_data.writeUtf8AsUtf16(name);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
#ifdef _MSC_VER
  {
      std::string connection_name;
      std::string binder_listen_addr;
      int r = ipc_connection_token_mgr::get_instance()
          .find_remote_service_by_service_name( name, connection_name, binder_listen_addr );
      if( r == 0 )
      {
          _aidl_data.writeUint32( ::android::OK );
      }
      else
      {
          _aidl_data.writeUint32( ::android::DEAD_OBJECT );
      }
      _aidl_data.writeUtf8AsUtf16( connection_name );
      _aidl_data.writeUtf8AsUtf16( binder_listen_addr );
  }
#else
  _aidl_ret_status = _aidl_data.writeStrongBinder(binder);
  if (((_aidl_ret_status) != (::android::OK))) {
    goto _aidl_error;
  }
#endif
  _aidl_ret_status = remote()->transact(BnServiceCallback::TRANSACTION_onRegistration, _aidl_data, &_aidl_reply, ::android::IBinder::FLAG_ONEWAY);
  if (UNLIKELY(_aidl_ret_status == ::android::UNKNOWN_TRANSACTION && IServiceCallback::getDefaultImpl())) {
     return IServiceCallback::getDefaultImpl()->onRegistration(name, binder);
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
#include <android/os/BnServiceCallback.h>
#include <binder/Parcel.h>
#include <binder/Stability.h>

namespace android {
namespace os {

BnServiceCallback::BnServiceCallback()
{
  ::android::internal::Stability::markCompilationUnit(this);
}

::android::status_t BnServiceCallback::onTransact(uint32_t _aidl_code, const ::android::Parcel& _aidl_data, ::android::Parcel* _aidl_reply, uint32_t _aidl_flags) {
  ::android::status_t _aidl_ret_status = ::android::OK;
  switch (_aidl_code) {
  case BnServiceCallback::TRANSACTION_onRegistration:
  {
    ::std::string in_name;
    ::android::sp<::android::IBinder> in_binder;
    if (!(_aidl_data.checkInterface(this))) {
      _aidl_ret_status = ::android::BAD_TYPE;
      break;
    }
    _aidl_ret_status = _aidl_data.readUtf8FromUtf16(&in_name);
    if (((_aidl_ret_status) != (::android::OK))) {
      break;
    }
#ifdef _MSC_VER
    std::string connection_name;
    std::string binder_listen_addr;
    uint32_t status = ::android::OK;
    _aidl_ret_status = _aidl_data.readUint32( &status );
    _aidl_ret_status = _aidl_data.readUtf8FromUtf16( &connection_name );
    _aidl_ret_status = _aidl_data.readUtf8FromUtf16( &binder_listen_addr );
    if( status == ::android::OK )
    {
        int id = ipc_connection_token_mgr::get_instance()
            .add_remote_service( in_name, connection_name, binder_listen_addr );
        in_binder = ProcessState::self()->getStrongProxyForHandle( id );
    }
#else
    _aidl_ret_status = _aidl_data.readStrongBinder(&in_binder);
    if (((_aidl_ret_status) != (::android::OK))) {
      break;
    }
#endif
    if (auto st = _aidl_data.enforceNoDataAvail(); !st.isOk()) {
      _aidl_ret_status = st.writeToParcel(_aidl_reply);
      break;
    }
    ::android::binder::Status _aidl_status(onRegistration(in_name, in_binder));
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
