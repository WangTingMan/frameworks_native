#include "ipc_connection_token.h"
#include <base/logging.h>
#include <base/files/file_path.h>
#include <base/strings/sys_string_conversions.h>
#include <android-base/properties.h>
#include <base/rand_util.h>

#ifdef _MSC_VER
#include <windows.h>
#endif

#define BINDER_LOW_LEVEL_LISTHEN_ADDRESS "persist.bluetooth.binder_listen_addr"

namespace android
{

ipc_connection_token_mgr& ipc_connection_token_mgr::get_instance()
{
    static ipc_connection_token_mgr instance;
    return instance;
}

ipc_connection_token_mgr::ipc_connection_token_mgr()
{
    std::string module_;
#ifdef _MSC_VER
    wchar_t module_name[MAX_PATH];
    GetModuleFileName( nullptr, module_name, MAX_PATH );
    ::base::FilePath path( module_name );
    module_ = path.RemoveFinalExtension().BaseName().StdStringValue();
    int pid = _getpid();
    module_.append("_").append( std::to_string(pid) );
    register_local_connection_name( module_ );
#endif

    LOG( INFO ) << "local ipc connection name is: " << module_;
}

void ipc_connection_token_mgr::add_local_service
    (
    std::string a_service_name,
    sp<RefBase> a_service_entity
    )
{
    local_service_entity entity;
    entity.m_service_entity = a_service_entity;
    entity.m_service_name = a_service_name;

    std::lock_guard<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_local_services )
    {
        if( ele.m_service_name == a_service_name )
        {
            LOG( ERROR ) << "Already registered: " << a_service_name;
            return;
        }
    }

    m_local_services.emplace_back( entity );
    LOG( INFO ) << "Registered local service: " << a_service_name;
}

sp<RefBase> ipc_connection_token_mgr::get_local_service( std::string const& a_name )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_local_services )
    {
        if( ele.m_service_name == a_name )
        {
            return ele.m_service_entity;
        }
    }

    return nullptr;
}

sp<RefBase> ipc_connection_token_mgr::get_local_service( std::vector<std::string> const& a_chain_name )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_local_services )
    {
        std::string_view name_view( ele.m_service_name );
        bool all_in = true;
        for( auto& ele : a_chain_name )
        {
            auto find_result = name_view.find( ele );
            if( find_result == std::string_view::npos )
            {
                all_in = false;
                break;
            }
        }

        if( all_in )
        {
            return ele.m_service_entity;
        }
    }

    return nullptr;
}

int ipc_connection_token_mgr::add_remote_callback
    (
    std::string a_callback_name,
    std::string a_remote_connection_name
    )
{
    std::lock_guard<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_registered_callback_interfaces )
    {
        if( ele.m_callback_name == a_callback_name &&
            ele.m_connection_name == a_remote_connection_name )
        {
            return ele.m_service_id;
        }
    }

    remote_registerd_callback cb;
    cb.m_service_id = m_next_remote_service_id++;
    cb.m_callback_name = a_callback_name;
    cb.m_connection_name = a_remote_connection_name;
    m_registered_callback_interfaces.push_back( cb );
    return cb.m_service_id;
}

int ipc_connection_token_mgr::add_remote_service
    (
    std::string a_service_name,
    std::string a_connection_name
    )
{
    std::lock_guard<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_remote_services )
    {
        if( ele.m_connection_name == a_connection_name &&
            ele.m_service_name == a_service_name )
        {
            return ele.m_service_id;
        }
    }

    remote_service_proxy proxy;
    proxy.m_connection_name = a_connection_name;
    proxy.m_service_name = a_service_name;
    proxy.m_service_id = m_next_remote_service_id++;
    m_remote_services.push_back( proxy );
    LOG( INFO ) << "add remote service. " << a_service_name << ", connection name: "
        << a_connection_name << ", id = " << proxy.m_service_id;
    return proxy.m_service_id;
}

int ipc_connection_token_mgr::add_remote_service
    (
    std::string a_service_name,
    std::string a_connection_name,
    std::string a_binder_listen_addr
    )
{
    std::lock_guard<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_remote_services )
    {
        if( ele.m_service_name == a_service_name )
        {
            ele.m_remote_binder_listen_addr = a_binder_listen_addr;
            ele.m_connection_name = a_connection_name;
            LOG( INFO ) << a_service_name << " already registered. we just update"
                << " linsten address: " << a_binder_listen_addr << ", and connection name: "
                << a_connection_name;
            return ele.m_service_id;
        }
    }

    remote_service_proxy proxy;
    proxy.m_connection_name = a_connection_name;
    proxy.m_service_name = a_service_name;
    proxy.m_service_id = m_next_remote_service_id++;
    proxy.m_remote_binder_listen_addr = a_binder_listen_addr;
    m_remote_services.push_back( proxy );
    LOG( INFO ) << "add remote service. " << a_service_name << ", connection name: "
        << a_connection_name << ", id = " << proxy.m_service_id << ", listhen on: "
        << a_binder_listen_addr;

    return proxy.m_service_id;
}

int ipc_connection_token_mgr::find_remote_service_id
    (
    std::string a_service_name,
    std::string a_connection_name
    )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_remote_services )
    {
        if( ele.m_connection_name == a_connection_name &&
            ele.m_service_name == a_service_name )
        {
            return ele.m_service_id;
        }
    }

    LOG( ERROR ) << "No such remote service " << a_service_name;
    return 0;
}

std::string ipc_connection_token_mgr::find_remote_service_listen_addr
    (
    int a_id
    )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_remote_services )
    {
        if( ele.m_service_id == a_id )
        {
            return ele.m_remote_binder_listen_addr;
        }
    }
    return "";
}

int ipc_connection_token_mgr::find_remote_service_by_id
    (
    int id, // [in]
    std::string& a_service_name, // [out]
    std::string& a_connection_name  // [out]
    )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    // find in remote services
    for( auto& ele : m_remote_services )
    {
        if( ele.m_service_id == id )
        {
            a_service_name = ele.m_service_name;
            a_connection_name = ele.m_connection_name;
            return 0;
        }
    }

    // find in remote registered callbacks
    for( auto& ele : m_registered_callback_interfaces )
    {
        if( ele.m_service_id == id )
        {
            a_service_name = ele.m_callback_name;
            a_connection_name = ele.m_connection_name;
            return 0;
        }
    }

    return -1;
}

int ipc_connection_token_mgr::find_remote_service_by_service_name
    (
    std::string a_service_name, // [in]
    std::string& a_connection_name, // [out]
    std::string& a_binder_listen_addr  // [out]
    )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_remote_services )
    {
        if( ele.m_service_name == a_service_name )
        {
            a_connection_name = ele.m_connection_name;
            a_binder_listen_addr = ele.m_remote_binder_listen_addr;
            return 0;
        }
    }
    return -1;
}

int ipc_connection_token_mgr::find_remote_service_by_service_name
    (
    std::vector<std::string> a_service_chain_name, // [in]
    std::string& a_connection_name, // [out]
    std::string& a_binder_listen_addr  // [out]
    )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_remote_services )
    {
        std::string_view name_view( ele.m_service_name );
        bool all_in = true;
        for( auto& ele : a_service_chain_name )
        {
            auto find_result = name_view.find( ele );
            if( find_result == std::string_view::npos )
            {
                all_in = false;
                break;
            }
        }

        if( all_in )
        {
            a_connection_name = ele.m_connection_name;
            a_binder_listen_addr = ele.m_remote_binder_listen_addr;
            return ele.m_service_id;
        }
    }
    return -1;
}

std::string ipc_connection_token_mgr::get_local_listen_address()
{
    std::string addr;
    addr = base::GetProperty( BINDER_LOW_LEVEL_LISTHEN_ADDRESS, "" );
    if( !addr.empty() )
    {
        return addr;
    }

    addr.assign( "127.0.0.1" );
    addr.append(":").append( std::to_string(::base::RandInt(10000, 30000)) );

    base::SetProperty( BINDER_LOW_LEVEL_LISTHEN_ADDRESS, addr );

    LOG( INFO ) << "Local service will listen on " << addr;
    return addr;
}

sp<RefBase> ipc_connection_token_mgr::get_registered_binder_interface( void* ptr )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_registered_local_binder_interfaces )
    {
        if( ele.get() == ptr )
        {
            return ele;
        }
    }
    return nullptr;
}

void ipc_connection_token_mgr::register_binder_interface( sp<RefBase> a_interface )
{
    std::lock_guard<std::shared_mutex> lcker( m_mutex );
    for( auto& ele : m_registered_local_binder_interfaces )
    {
        if( ele.get() == a_interface.get() )
        {
            return;
        }
    }
    m_registered_local_binder_interfaces.push_back( a_interface );
}

void ipc_connection_token_mgr::set_current_transaction_connection_name( std::string a_name )
{
    uint64_t tid = GetCurrentThreadId();
    std::lock_guard<std::shared_mutex> lcker( m_mutex );
    m_cached_connection_name[tid] = a_name;
}

std::string ipc_connection_token_mgr::get_current_transaction_connection_name( bool a_remove )
{
    uint64_t tid = GetCurrentThreadId();
    std::string name;
    std::lock_guard<std::shared_mutex> lcker( m_mutex );
    auto it = m_cached_connection_name.find( tid );
    if( it != m_cached_connection_name.end() )
    {
        name = it->second;
        if( a_remove )
        {
            m_cached_connection_name.erase( it );
        }
    }
    return name;
}

void ipc_connection_token_mgr::set_service_manager_connection_name( std::string const& a_name )
{
    std::lock_guard<std::shared_mutex> lcker( m_mutex );
    if( m_service_manager_connection_name.empty() )
    {
        m_service_manager_connection_name = a_name;
    }
}

std::string const& ipc_connection_token_mgr::get_service_manager_connection_name()const noexcept
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    return m_service_manager_connection_name;
}


}

