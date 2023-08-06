#pragma once
#include <atomic>
#include <condition_variable>
#include <list>
#include <memory>
#include <cstdint>
#include <vector>
#include <shared_mutex>
#include <set>

#include <linux/libbinder_driver_exports.h>
#include <linux/binder.h>

#include "parcel_porting.h"
#include "parcel_writer_interface.h"

#include "data_link/server.h"
#include "data_link/client.h"

class binder_internal_control_block
{

public:

    friend class binder_internal_control_block_mgr;

    void set_oneway_spam_detection_enabled(bool a_enable = true)
    {
        std::lock_guard<std::recursive_mutex> locker(m_mutex);
        oneway_spam_detection_enabled = a_enable;
    }

    uint32_t get_id()const
    {
        std::lock_guard<std::recursive_mutex> locker(m_mutex);
        return id;
    }

    int handle_write_read_block(binder_write_read* a_wr_blk);

private:

    void handle_incoming_ipc_message( std::shared_ptr<data_link::binder_ipc_message> a_msg );

    int handle_transaction(binder_write_read* a_wr_blk, std::shared_ptr<android::parcel_writer_interface>& a_viewer_mOut);

    int handle_add_service( binder_write_read* a_wr_blk, binder_transaction_data& a_write_cur_ptr );

    int handle_ping_service_manager( binder_write_read* a_wr_blk, binder_transaction_data& a_write_cur_ptr );

    /**
     * Check connection is ready or not. Make a connection and try to connect
     * if not ready.
     */
    bool check_connection_ready();

    bool check_receivd_message( data_link::binder_message_type a_msg );

    void wait_for_contion_variable();

    mutable std::recursive_mutex m_mutex;
    std::condition_variable_any m_condition_var;
    bool oneway_spam_detection_enabled = false;
    uint32_t id = 0;
    std::shared_ptr<data_link::binder_ipc_message> m_received_binder_message;
    std::shared_ptr<data_link::client> m_client_to_binder_serivce_manager;
};


