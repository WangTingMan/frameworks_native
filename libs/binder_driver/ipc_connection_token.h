#pragma once

#include "linux/libbinder_driver_exports.h"

#include <map>
#include <shared_mutex>
#include <string>
#include <vector>
#include <utils/RefBase.h>

namespace android
{

class local_service_entity
{

public:

    std::string m_service_name;
    sp<RefBase> m_service_entity;
};

class remote_service_proxy
{

public:

    int32_t m_service_id = 0;
    std::string m_service_name;
    std::string m_connection_name;
    std::string m_remote_binder_listen_addr;
};

class remote_registerd_callback
{

public:

    int32_t m_service_id = 0;
    std::string m_callback_name;    // The callback name
    std::string m_connection_name;  // Which remote connection name registered this callback
};

using remote_die_callback_type = std::function<void(
    std::string /*a_service_name*/,
    std::string /*a_connection_name*/,
    std::string /*a_binder_listen_addr*/ )>;

struct remote_die_callback_cb
{
    remote_die_callback_type m_callback;
    uint32_t m_id = 0;
};

/**
 * manager local and remote services.
 * For local service, will hold the service entity
 */
class LIBBINDERDRIVER_EXPORTS ipc_connection_token_mgr
{

public:

    static constexpr char s_name_separator = '/';

    static ipc_connection_token_mgr& get_instance();

    void register_local_connection_name( std::string a_name )
    {
        std::lock_guard<std::shared_mutex> lcker( m_mutex );
        m_local_name = a_name;
    }

    std::string const& get_local_connection_name()const noexcept
    {
        std::shared_lock<std::shared_mutex> lcker( m_mutex );
        return m_local_name;
    }

    bool is_connection_name_exist( std::string const& a_connection_name );

    void add_local_service
        (
        std::string a_service_name,
        sp<RefBase> a_service_entity
        );

    sp<RefBase> get_local_service( std::string const& a_name );

    sp<RefBase> get_local_service( std::vector<std::string> const& a_chain_name );

    int add_remote_callback
        (
        std::string a_callback_name,
        std::string a_remote_connection_name
        );

    int add_remote_service
        (
        std::string a_service_name,
        std::string a_connection_name
        );

    int add_remote_service
        (
        std::string a_service_name,
        std::string a_connection_name,
        std::string a_binder_listen_addr
        );

    /**
     * Register remote die callback. return the id of this callback.
     * Use id to unregister this callback
     */
    uint32_t register_remote_die_callback( remote_die_callback_type a_callback );

    void unregister_remote_die_callback( uint32_t a_id );

    /**
     * Remove all service and callback identified by a_connection_name.
     * Call this API if a_connection_name has been disconnected or something.
     */
    int remove_all_remote_service
        (
        std::string a_connection_name,
        bool a_remote_die = true
        );

    int find_remote_service_id
        (
        std::string a_service_name,
        std::string a_connection_name
        );

    std::string find_remote_service_listen_addr
        (
        int a_id
        );

    int find_remote_service_by_id
        (
        int id, // [in]
        std::string& a_service_name, // [out]
        std::string& a_connection_name  // [out]
        );

    int find_remote_service_by_service_name
        (
        std::string a_service_name, // [in]
        std::string& a_connection_name, // [out]
        std::string& a_binder_listen_addr  // [out]
        );

    int find_remote_service_by_service_name
        (
        std::vector<std::string> a_service_chain_name, // [in]
        std::string& a_connection_name, // [out]
        std::string& a_binder_listen_addr  // [out]
        );

    std::string get_local_listen_address();

    sp<RefBase> get_registered_binder_interface( void* ptr );

    void register_binder_interface( sp<RefBase> a_interface );

    /**
     * Sometimes the we cannot pass connection name into parameters.
     * So we just set current transaction name here. And it can be
     * retrieve later.
     */
    void set_current_transaction_connection_name( std::string a_name );

    std::string get_current_transaction_connection_name( bool a_remove = false );

    void set_service_manager_connection_name( std::string const& a_name );

    std::string const& get_service_manager_connection_name()const noexcept;

    bool get_debug_enabled()const
    {
        return m_debug_enabled;
    }

private:

    ipc_connection_token_mgr();

    ~ipc_connection_token_mgr();

    bool m_debug_enabled = false;

    mutable std::shared_mutex m_mutex;
    std::string m_local_name;
    std::string m_service_manager_connection_name;
    int m_next_remote_service_id = 10;
    std::vector<local_service_entity> m_local_services;
    std::vector<remote_service_proxy> m_remote_services;
    std::vector<sp<RefBase>> m_registered_local_binder_interfaces;
    std::vector<remote_registerd_callback> m_registered_callback_interfaces;
    uint32_t m_next_callback_id = 0;
    std::map<uint64_t, std::string> m_cached_connection_name;
    std::vector<remote_die_callback_cb> m_remote_die_callbacks;
};

}

