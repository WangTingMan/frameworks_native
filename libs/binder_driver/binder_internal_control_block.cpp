#include <linux/binder_internal_control_block.h>
#include <linux/binder_internal_control_block_mgr.h>
#include <linux/binder.h>
#include <utils/Errors.h>

#include <base/logging.h>
#include "parcel_writer_interface.h"

static constexpr uint32_t TRANSACTION_base_value = static_cast<uint32_t>(BinderCode::FIRST_CALL_TRANSACTION);
static constexpr uint32_t TRANSACTION_getService = TRANSACTION_base_value + 0;
static constexpr uint32_t TRANSACTION_checkService = TRANSACTION_base_value + 1;
static constexpr uint32_t TRANSACTION_addService = TRANSACTION_base_value + 2;
static constexpr uint32_t TRANSACTION_listServices = TRANSACTION_base_value + 3;
static constexpr uint32_t TRANSACTION_registerForNotifications = TRANSACTION_base_value + 4;
static constexpr uint32_t TRANSACTION_unregisterForNotifications = TRANSACTION_base_value + 5;
static constexpr uint32_t TRANSACTION_isDeclared = TRANSACTION_base_value + 6;
static constexpr uint32_t TRANSACTION_getDeclaredInstances = TRANSACTION_base_value + 7;
static constexpr uint32_t TRANSACTION_updatableViaApex = TRANSACTION_base_value + 8;
static constexpr uint32_t TRANSACTION_getConnectionInfo = TRANSACTION_base_value + 9;
static constexpr uint32_t TRANSACTION_registerClientCallback = TRANSACTION_base_value + 10;
static constexpr uint32_t TRANSACTION_tryUnregisterService = TRANSACTION_base_value + 11;
static constexpr uint32_t TRANSACTION_getServiceDebugInfo = TRANSACTION_base_value + 12;

int binder_internal_control_block::handle_write_read_block(binder_write_read* a_wr_blk)
{
    int status = 0;
    uint8_t* ptr = reinterpret_cast<uint8_t*>(a_wr_blk->write_buffer) + a_wr_blk->write_consumed;
    uint32_t cmd = 0;

    std::shared_ptr<android::parcel_writer_interface> viewer_mOut;
    viewer_mOut = android::get_parcel_writer_maker()();
    viewer_mOut->ipcSetDataReference( ptr, a_wr_blk->write_size, nullptr, 0 );
    cmd = viewer_mOut->readInt32();

    switch (cmd) {
    case BC_INCREFS:
    {
        int32_t handle = 0;
        memcpy(&handle, ptr, sizeof(int32_t));
        a_wr_blk->write_consumed = sizeof(cmd) + sizeof(handle);
    }
        break;
    case BC_ACQUIRE:
    {
        int32_t handle = 0;
        memcpy(&handle, ptr, sizeof(int32_t));
        a_wr_blk->write_consumed = sizeof(cmd) + sizeof(handle);
    }
        break;
    case BC_RELEASE:
        LOG(ERROR) << "No implementation!";
        break;
    case BC_TRANSACTION:
    case BC_REPLY:
    {
        status = handle_transaction( a_wr_blk, viewer_mOut );
        a_wr_blk->write_consumed = a_wr_blk->write_size;
        break;
    }
    case BC_FREE_BUFFER:
        {
            char* buffer = (char*)viewer_mOut->readUint64();
            binder_internal_control_block_mgr::get_instance().return_back_buffer( buffer );
            a_wr_blk->write_consumed = a_wr_blk->write_size;
        }
        break;
    case BC_ENTER_LOOPER:
        a_wr_blk->write_consumed = sizeof(cmd);
        binder_internal_control_block_mgr::get_instance().enter_looper();
        break;
    default:
        LOG(ERROR) << "No implementation with " << cmd;
    }

    return status;
}

void binder_internal_control_block::handle_incoming_ipc_message
    (
    std::shared_ptr<data_link::binder_ipc_message> a_msg
    )
{
    std::lock_guard<std::recursive_mutex> lcker( m_mutex );
    m_received_binder_message = a_msg;
    m_condition_var.notify_all();
}

int binder_internal_control_block::handle_transaction
    (
    binder_write_read* a_wr_blk,
    std::shared_ptr<android::parcel_writer_interface>& a_viewer_mOut
    )
{
    int status = 0;
    binder_transaction_data tr;
    a_viewer_mOut->read( (void*)( &tr ), sizeof( binder_transaction_data ) );
    switch (tr.code)
    {
    case static_cast<uint32_t>(BinderCode::PING_TRANSACTION):
        status = handle_ping_service_manager( a_wr_blk, tr);
        break;
    case TRANSACTION_addService:
        status = handle_add_service( a_wr_blk, tr );
        break;
    default:
        LOG(ERROR) << "No implementation!";
    }
    return status;
}

int binder_internal_control_block::handle_add_service( binder_write_read* a_wr_blk, binder_transaction_data& a_transaction_data )
{
    uint32_t consumed_size = 0;
    consumed_size += sizeof( uint32_t );
    consumed_size += sizeof( binder_transaction_data );
    a_wr_blk->read_consumed = consumed_size; // We wrote consumed_size bytes to read buffer.

    std::shared_ptr<android::parcel_writer_interface> viewer;
    viewer = android::get_parcel_writer_maker()( );
    viewer->ipcSetDataReference(
        reinterpret_cast<const uint8_t*>( a_transaction_data.data.ptr.buffer ),
        a_transaction_data.data_size,
        reinterpret_cast<const binder_size_t*>( a_transaction_data.data.ptr.offsets ),
        a_transaction_data.offsets_size / sizeof( binder_size_t ));

    android::String8 token = viewer->readString8();
    if( token != android::get_next_pointer_key() )
    {
        LOG( FATAL ) << "Not next pointer key.";
        return 0;
    }
    uint64_t p_int = viewer->readUint64();
    if( !p_int )
    {
        LOG( FATAL ) << "Not set AddServiceControlBlock pointer.";
        return 0;
    }

    std::shared_ptr<android::AddServiceControlBlock> cb_ptr(
        (android::AddServiceControlBlock*)( p_int ),
        [](void* ptr )
        {
            android::AddServiceControlBlock* detail_ptr;
            detail_ptr = reinterpret_cast<android::AddServiceControlBlock*>( ptr );
            android::release_control_block( detail_ptr );
        });
    binder_internal_control_block_mgr::get_instance().register_service_local( cb_ptr );
    LOG( INFO ) << "Start register service: " << cb_ptr->service_name;

    // send register message to service manager.
    if( !check_connection_ready() )
    {
        LOG( FATAL ) << "before register service, need create client to service manager and ping it";
        return -1;
    }
    std::shared_ptr<data_link::register_service_message> send_binder_message;
    send_binder_message = std::make_shared<data_link::register_service_message>();
    send_binder_message->m_service_name = cb_ptr->service_name;
    m_client_to_binder_serivce_manager->send_message( send_binder_message );
    for( ;; wait_for_contion_variable() )
    {
        // 1. check connection if OK or not.
        if( !check_connection_ready() )
        {
            ALOGE( "connection broken when waiting for response" );
            return -1;
        }

        // 2. check incoming message
        if( !check_receivd_message( data_link::binder_message_type::register_service ) )
        {
            continue;
        }
        break;
    }

    std::shared_ptr<android::parcel_writer_interface> parcel_writer;
    parcel_writer = android::get_parcel_writer_maker()();
    parcel_writer->write_int32_t( BR_REPLY );

    // prepare upper layer reply message.
    binder_transaction_data tr;
    std::shared_ptr<android::parcel_writer_interface> upper_reply_writer;
    upper_reply_writer = android::get_parcel_writer_maker()();
    upper_reply_writer->write_int32_t( android::OK );
    std::string write_msg{ "add service:" };
    write_msg.append( cb_ptr->service_name );
    upper_reply_writer->writeUtf8AsUtf16( write_msg );
    char* ping_buffer = nullptr;
    ping_buffer = binder_internal_control_block_mgr::get_instance().get_buffer(
        upper_reply_writer->size() + 1 );
    upper_reply_writer->copy_to( (uint8_t*)ping_buffer, upper_reply_writer->size() + 1 );
    memset( &tr, 0x00, sizeof( binder_transaction_data ) );
    tr.data.ptr.buffer = reinterpret_cast<binder_uintptr_t>( ping_buffer );
    tr.data_size = upper_reply_writer->size() + 1;
    parcel_writer->write( &tr, sizeof( binder_transaction_data ) );

    uint8_t* read_ptr = reinterpret_cast<uint8_t*>( a_wr_blk->read_buffer );
    if( read_ptr )
    {
        parcel_writer->copy_to( read_ptr, parcel_writer->size() );
        a_wr_blk->read_consumed = parcel_writer->size();
    }

    LOG( INFO ) << "service " << cb_ptr->service_name << " registered.";

    return 0;
}

int binder_internal_control_block::handle_ping_service_manager( binder_write_read* a_wr_blk, binder_transaction_data& a_write_cur_ptr )
{
    int status = 0;
    uint8_t* read_ptr = reinterpret_cast<uint8_t*>( a_wr_blk->read_buffer );
    if( !read_ptr )
    {
        LOG( FATAL ) << "read_ptr is nullptr!";
        return status;
    }

    LOG( INFO ) << "handle ping service manager.";
    for( ;; wait_for_contion_variable() )
    {
        // 1. check connection if OK or not.
        if( !check_connection_ready() )
        {
            continue;
        }

        break;
    }

    std::shared_ptr<data_link::binder_ping_message> send_binder_message;
    send_binder_message = std::make_shared<data_link::binder_ping_message>();
    send_binder_message->m_ping_message.assign( "client ping message." );

    m_client_to_binder_serivce_manager->send_message( send_binder_message );

    for( ;; wait_for_contion_variable() )
    {
        // 1. check connection if OK or not.
        if( !check_connection_ready() )
        {
            status = -1;
            ALOGE( "connection broken when waiting for response" );
            return status;
        }

        // 2. check incoming message
        if( !check_receivd_message( data_link::binder_message_type::ping_service_manager ) )
        {
            continue;
        }
        break;
    }

    LOG( INFO ) << "received ping reply from service manager";

    std::shared_ptr<android::parcel_writer_interface> parcel_writer;
    parcel_writer = android::get_parcel_writer_maker()( );
    parcel_writer->write_int32_t( BR_REPLY );

    binder_transaction_data tr;
    memset( &tr, 0x00, sizeof( binder_transaction_data ) );
    auto ping_msg_reply = std::dynamic_pointer_cast<data_link::binder_ping_message>(
        m_received_binder_message );
    std::string ping_reply( "Hello, binder!" );
    if( ping_msg_reply )
    {
        ping_reply = ping_msg_reply->m_ping_message;
    }
    char* ping_buffer = nullptr;
    ping_buffer = binder_internal_control_block_mgr::get_instance().get_buffer(
        ping_reply.size() + 1 );
    strncpy_s( ping_buffer, ping_reply.size() + 1, ping_reply.c_str(), ping_reply.size() );
    tr.data.ptr.buffer = reinterpret_cast<binder_uintptr_t>( ping_buffer );
    tr.data_size = ping_reply.size() + 1;
    parcel_writer->write( &tr, sizeof( binder_transaction_data ) );

    parcel_writer->copy_to( read_ptr, parcel_writer->size() );
    a_wr_blk->read_consumed = parcel_writer->size();
    m_received_binder_message.reset();
    return 0;
}

bool binder_internal_control_block::check_connection_ready()
{
    bool connection_ready = false;
    if( m_client_to_binder_serivce_manager )
    {
        auto status = m_client_to_binder_serivce_manager->get_connection_status();
        if( status == data_link::connection_status::connected )
        {
            return true;
        }
    }
    else
    {
        m_client_to_binder_serivce_manager = data_link::client::make_one();
        m_client_to_binder_serivce_manager->register_message_incoming_callback
            (
            std::bind( &binder_internal_control_block::handle_incoming_ipc_message,
                this, std::placeholders::_1)
            );
    }

    if( data_link::connection_status::disconnected ==
        m_client_to_binder_serivce_manager->get_connection_status() )
    {
        std::string service_manager_addr = binder_internal_control_block_mgr::get_instance()
            .get_service_manager_endpoint();
        m_client_to_binder_serivce_manager->connect( service_manager_addr );
    }

    return connection_ready;
}

bool binder_internal_control_block::check_receivd_message( data_link::binder_message_type a_msg_type )
{
    std::lock_guard<std::recursive_mutex> lcker( m_mutex );
    if( m_received_binder_message )
    {
        if( m_received_binder_message->get_type() == a_msg_type )
        {
            return true;
        }
    }
    return false;
}

void binder_internal_control_block::wait_for_contion_variable()
{
    std::chrono::seconds wait_time{ 1 };
    std::unique_lock<std::recursive_mutex> lcker( m_mutex );
    m_condition_var.wait_for( lcker, wait_time );
}
