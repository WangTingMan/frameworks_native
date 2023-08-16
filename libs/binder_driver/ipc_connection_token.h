#pragma once

#include "linux/libbinder_driver_exports.h"

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

/**
 * manager local and remote services.
 * For local service, will hold the service entity
 */
class LIBBINDERDRIVER_EXPORTS ipc_connection_token_mgr
{

public:

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

    void add_local_service
        (
        std::string a_service_name,
        sp<RefBase> a_service_entity
        );

    sp<RefBase> get_local_service( std::string const& a_name );

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

    std::string get_local_listen_address();

    sp<RefBase> get_registered_binder_interface( void* ptr );

    void register_binder_interface( sp<RefBase> a_interface );

private:

    ipc_connection_token_mgr();

    mutable std::shared_mutex m_mutex;
    std::string m_local_name;
    int m_next_remote_service_id = 10;
    std::vector<local_service_entity> m_local_services;
    std::vector<remote_service_proxy> m_remote_services;
    std::vector<sp<RefBase>> m_registered_local_binder_interfaces;
};

}

