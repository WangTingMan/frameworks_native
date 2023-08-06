#pragma once
#include <linux/libbinder_driver_exports.h>
#include <linux/binder.h>

#include <atomic>
#include <list>
#include <memory>
#include <cstdint>
#include <vector>
#include <shared_mutex>
#include <set>

#include "data_link/server.h"
#include "data_link/client.h"

class binder_internal_control_block;

namespace android
{
    struct AddServiceControlBlock;
}

class LIBBINDERDRIVER_EXPORTS binder_internal_control_block_mgr
{

public:

    static binder_internal_control_block_mgr& get_instance();

    std::shared_ptr<binder_internal_control_block> find_by_id( uint32_t a_id );

    std::shared_ptr<binder_internal_control_block> create_new_one();

    char* get_buffer( uint32_t a_size_expect );

    void return_back_buffer( char* );

    bool remove_control_block( uint32_t a_id );

    void enter_looper();

    void set_service_manager( bool is_manager )
    {
        m_is_service_manager = is_manager;
    }

    std::string get_service_manager_endpoint()
    {
        return "127.0.0.1:5151";
    }

    bool register_service_local( std::shared_ptr<android::AddServiceControlBlock> a_service );

private:

    void handle_incoming_message
        (
        std::shared_ptr<data_link::binder_ipc_message> a_message 
        );

    void handle_ping_message
        (
        std::shared_ptr<data_link::binder_ping_message> a_message
        );

    void handle_register_service_message
        (
        std::shared_ptr<data_link::register_service_message> a_message
        );

    static std::atomic_uint32_t m_next_id;

    struct buffer_compare
    {
        bool operator()
            (
                std::shared_ptr<std::vector<char>> const& left,
                std::shared_ptr<std::vector<char>> const& right
                )const
        {
            return left->data() < right->data();
        }
    };

    std::shared_mutex m_mutex;
    std::vector<std::shared_ptr<binder_internal_control_block>> m_blocks;
    std::set<std::shared_ptr<std::vector<char>>, buffer_compare> m_used_buffers;
    std::vector<std::shared_ptr<std::vector<char>>> m_free_buffers;
    std::shared_ptr<data_link::server> m_server;
    std::vector<std::shared_ptr<android::AddServiceControlBlock>> mAddedServices;
    bool m_is_service_manager = false; // Current process is the service manager or not
};
