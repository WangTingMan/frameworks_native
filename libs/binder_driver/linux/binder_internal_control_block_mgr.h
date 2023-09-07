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

#include <parcel_writer_interface.h>

struct client_to_normal_service
{
    int m_id;
    std::shared_ptr<data_link::client> m_client_to_normal_service;
};

class client_control_block;

namespace data_link
{
    class binder_ipc_message;
}

class LIBBINDERDRIVER_EXPORTS binder_internal_control_block_mgr
{

public:

    static binder_internal_control_block_mgr& get_instance();

    uint32_t get_fake_fd()
    {
        return 512;
    }

    void shut_down( uint32_t )
    {

    }

    /**
     * Ask manager to allocate a buffer area
     */
    char* get_buffer( uint32_t a_size_expect );

    /**
     * Return the buffer to this manager.
     */
    void return_back_buffer( char* );

    void enter_looper();

    /**
     * Set local process is service manager or not.
     * If no one invoke this API, then won't be service manager.
     */
    void set_service_manager( bool is_manager )
    {
        m_is_service_manager = is_manager;
    }

    /**
     * Return is service manager or not.
     */
    bool is_service_manager()
    {
        return m_is_service_manager;
    }

    void set_binder_data_handler( std::function<void()> a_fun, bool a_for_aidl )
    {
        std::lock_guard<std::recursive_mutex> lcker( m_mutex );
        if( a_for_aidl )
        {
            m_binder_data_handler = a_fun;
        }
        else
        {
            m_hidl_data_handler = a_fun;
        }
    }

    std::string get_service_manager_endpoint()
    {
        return "127.0.0.1:5151";
    }

    int handle_read_only( binder_write_read* a_wr_blk );

    int handle_write_read_block( binder_write_read* a_wr_blk );

    void set_oneway_spam_detection_enabled( bool a_enable = true )
    {
        std::lock_guard<std::recursive_mutex> locker( m_mutex );
        oneway_spam_detection_enabled = a_enable;
    }

    void start_local_link_server();

    void invoke_binder_data_handler();

    void invoke_hidl_data_handler();

    void set_previous_hanlded_messsge( std::shared_ptr<data_link::binder_ipc_message> a_msg )
    {
        m_previous_handles_message = a_msg;
    }

    std::shared_ptr<data_link::binder_ipc_message> get_previous_handle_message();

private:

    void handle_transaction_sg
        (
        binder_write_read* a_wr_blk,
        binder_transaction_data_sg a_tc_sg
        );

    void send_reply_only
        (
        binder_write_read* a_wr_blk,
        binder_transaction_data& a_write_cur_ptr
        );

    void handle_new_client_incoming( std::shared_ptr<data_link::client> a_client );

    std::shared_ptr<client_control_block> find_client( uint32_t a_handle );

    std::shared_ptr<client_control_block> find_client( std::string const& a_listen_addr );

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

    std::function<void()> m_binder_data_handler; // binder message routing
    std::function<void()> m_hidl_data_handler;   // hidl message routing
    std::set<std::shared_ptr<std::vector<char>>, buffer_compare> m_used_buffers;
    std::vector<std::shared_ptr<std::vector<char>>> m_free_buffers;
    std::shared_ptr<data_link::server> m_server;
    mutable std::recursive_mutex m_mutex;
    bool oneway_spam_detection_enabled = false;
    bool m_is_service_manager = false; // Current process is the service manager or not
    std::vector<std::shared_ptr<client_control_block>> m_clients;
    std::shared_ptr<data_link::binder_ipc_message> m_previous_handles_message; // just for debug using.
};
