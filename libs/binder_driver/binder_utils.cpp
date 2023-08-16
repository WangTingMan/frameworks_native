#include <binder_utils.h>
#include <linux\binder.h>

std::string tr_code_to_string( uint32_t a_tr_code )
{
    switch( a_tr_code )
    {
    case static_cast< uint32_t >( BinderCode::PING_TRANSACTION ):
        return "BinderCode::PING_TRANSACTION";
        break;
    case TRANSACTION_checkService:
        return "TRANSACTION_checkService";
    case TRANSACTION_addService:
        return "TRANSACTION_addService";
    case TRANSACTION_listServices:
        return "TRANSACTION_listServices";
    case TRANSACTION_registerForNotifications:
        return "TRANSACTION_registerForNotifications";
    case TRANSACTION_unregisterForNotifications:
        return "TRANSACTION_unregisterForNotifications";
    case TRANSACTION_isDeclared:
        return "TRANSACTION_isDeclared";
    case TRANSACTION_getDeclaredInstances:
        return "TRANSACTION_getDeclaredInstances";
    case TRANSACTION_updatableViaApex:
        return "TRANSACTION_updatableViaApex";
    case TRANSACTION_getConnectionInfo:
        return "TRANSACTION_getConnectionInfo";
    case TRANSACTION_registerClientCallback:
        return "TRANSACTION_registerClientCallback";
    case TRANSACTION_tryUnregisterService:
        return "TRANSACTION_tryUnregisterService";
    case TRANSACTION_getServiceDebugInfo:
        return "TRANSACTION_getServiceDebugInfo";
        break;
    case static_cast< uint32_t >( BinderCode::FIRST_CALL_TRANSACTION ):
        return "BinderCode::FIRST_CALL_TRANSACTION";
        break;
    case static_cast< uint32_t >( BinderCode::INTERFACE_TRANSACTION ):
        return "BinderCode::INTERFACE_TRANSACTION";
        break;
    default:
        return "unknown tr code.";
    }

    return "";
}

