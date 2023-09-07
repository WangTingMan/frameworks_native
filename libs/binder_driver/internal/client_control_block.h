#pragma once

#include "data_link/client.h"
#include <linux/binder.h>
#include <parcel_writer_interface.h>

#include <mutex>
#include <condition_variable>

class client_control_block
{


public:

    client_control_block( uint32_t a_service_id );

    client_control_block( std::shared_ptr<data_link::client> a_client );

    int handle_transaction
        (
        binder_write_read* a_wr_blk,
        binder_transaction_data& a_write_cur_ptr
        );

    void send_reply_only
        (
        binder_write_read* a_wr_blk,
        binder_transaction_data& a_write_cur_ptr
        );

    int handle_ping_service_manager
        (
        binder_write_read* a_wr_blk,
        binder_transaction_data& a_write_cur_ptr
        );

    int handle_general_transaction
        (
        binder_write_read* a_wr_blk,
        binder_transaction_data& a_write_cur_ptr
        );

    int handle_binder_message_sending
        (
        binder_write_read* a_wr_blk,
        binder_transaction_data& a_write_cur_ptr
        );

    void wait_for_contion_variable();

    bool check_connection_ready();

    std::shared_ptr<data_link::binder_ipc_message>
        check_receivd_message
        (
        data_link::binder_message_type a_msg_type
        );

    std::shared_ptr<data_link::binder_transaction_message>
        check_received_transaction_message
        (
        uint64_t a_id,
        std::string const& a_service_name,
        std::string const& a_source_connection_name
        );

    void handle_incoming_ipc_message( binder_write_read* a_wr_blk );

    uint32_t get_service_id()
    {
        std::lock_guard<std::recursive_mutex> locker( m_mutex );
        return m_service_id;
    }

    /**
     * Get the size of incoming message which need to handle
     */
    uint32_t get_incoming_message_size()
    {
        std::lock_guard<std::recursive_mutex> lcker( m_mutex );
        return m_received_binder_messages.size();
    }

    bool has_trasaction_need_reply()
    {
        return !m_transaction_in_process_msgs.empty();
    }

    // only used for if we are client proxy( we init the connection )
    std::string const& remote_listen_address()const noexcept
    {
        return m_remote_address;
    }

private:

    /**
    * Receive incomint ipc message and push it into list( not handle it )
    */
    void receive_incoming_ipc_message
        (
        std::shared_ptr<data_link::binder_ipc_message> a_msg
        );

    void handle_connection_status_changed( data_link::connection_status a_status );

    void handle_ping_message
        (
        std::shared_ptr<data_link::binder_ping_message> a_message
        );

    /**
     * Handle the coming transaction message sent by remote device
     */
    void handle_general_transaction_message
        (
        binder_write_read* a_wr_blk,
        std::shared_ptr<data_link::binder_transaction_message> a_message
        );

    void init_client_to_service();

    std::shared_ptr<data_link::binder_transaction_message>
        get_processing_transaction
        (
        uint64_t a_message_id,
        std::string a_service_name,
        std::string a_source_connection_name
        );

    std::condition_variable_any m_condition_var;
    mutable std::recursive_mutex m_mutex;
    std::shared_ptr<data_link::client> m_client;
    std::vector<std::shared_ptr<data_link::binder_ipc_message>> m_received_binder_messages;
    std::vector<std::shared_ptr<data_link::binder_transaction_message>> m_transaction_in_process_msgs;
    std::vector<std::shared_ptr<data_link::binder_ipc_message>> m_received_hidl_messages;
    uint32_t m_service_id = 0;
    std::function<void()> m_binder_data_handler;
    std::function<void( data_link::connection_status )> m_connection_notify_internal;
    std::string m_remote_connection_name; // which connection name we are connected to
    std::string m_remote_address; // peer address if we are client proxy
};

