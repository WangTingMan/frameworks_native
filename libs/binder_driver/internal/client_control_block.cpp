#include "client_control_block.h"
#include <binder_utils.h>
#include <linux/binder_internal_control_block_mgr.h>
#include <base/logging.h>
#include <data_link/message_types.h>
#include <ipc_connection_token.h>

#include <future>

client_control_block::client_control_block( std::shared_ptr<data_link::client> a_client )
    : m_client( a_client )
{
    m_client->register_message_incoming_callback
        (
        std::bind( &client_control_block::receive_incoming_ipc_message,
            this, std::placeholders::_1 )
        );
    m_client->register_connection_status_callback( std::bind(
        &client_control_block::handle_connection_status_changed, this, std::placeholders::_1 ) );
}

client_control_block::client_control_block( uint32_t a_service_id )
    : m_service_id( a_service_id )
{
    init_client_to_service();
}

int client_control_block::handle_transaction
    (
    binder_write_read* a_wr_blk,
    binder_transaction_data& a_tr
    )
{
    int status = 0;
    if( a_tr.code == 0 &&
        a_tr.target.binder_handle == -1 )
    {
        LOG( ERROR ) << "tr.code = 0 and binder_handle = -1";
    }

    if( a_tr.code == static_cast< uint32_t >( BinderCode::PING_TRANSACTION ) )
    {
        status = handle_ping_service_manager( a_wr_blk, a_tr );
    }
    else
    {
        status = handle_general_transaction( a_wr_blk, a_tr );
    }

    if( a_tr.is_aidl_transaction )
    {
        if( a_tr.code == TRANSACTION_addService &&
            a_tr.target.binder_handle == 0 )
        {
            // since we are going to add local service, then we need start local listen server.
            // binder handle == 0, which means register service into service manager.
            binder_internal_control_block_mgr::get_instance().start_local_link_server();
        }
    }
    else
    {
        if( a_tr.code == 12 &&
            a_tr.target.binder_handle == 0 )
        {
            // since we are going to add local service, then we need start local listen server.
            // binder handle == 0, which means register service into service manager.
            binder_internal_control_block_mgr::get_instance().start_local_link_server();
        }
    }

    return status;
}

int client_control_block::handle_ping_service_manager
    (
    binder_write_read* a_wr_blk,
    binder_transaction_data& a_write_cur_ptr
    )
{

    int status = 0;
    uint8_t* read_ptr = reinterpret_cast< uint8_t* >( a_wr_blk->read_buffer );
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

    m_client->send_message( send_binder_message );
    std::shared_ptr<data_link::binder_ipc_message> ret_msg;
    std::string current_transaction_name;
    current_transaction_name = android::ipc_connection_token_mgr::get_instance()
        .get_current_transaction_connection_name();
    uint64_t waiting_id = send_binder_message->get_id();
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
        ret_msg = check_receivd_message( current_transaction_name, waiting_id, data_link::binder_message_type::ping_service_manager,
                                         a_write_cur_ptr.is_aidl_transaction );
        if( !ret_msg )
        {
            continue;
        }
        break;
    }

    LOG( INFO ) << "received ping reply from service manager";

    std::shared_ptr<android::parcel_writer_interface> parcel_writer;
    parcel_writer = a_write_cur_ptr.is_aidl_transaction ? android::get_parcel_writer_maker()()
        : android::get_hidl_parcel_writer_maker()();
    parcel_writer->write_int32_t( BR_REPLY );

    binder_transaction_data tr;
    memset( &tr, 0x00, sizeof( binder_transaction_data ) );
    auto ping_msg_reply = std::dynamic_pointer_cast<data_link::binder_ping_message>( ret_msg );
    std::string ping_reply( "Hello, binder!" );
    if( ping_msg_reply )
    {
        ping_reply = ping_msg_reply->m_ping_message;
    }
    else
    {
        LOG( FATAL ) << "Received message is not ping message reply!";
    }
    char* ping_buffer = nullptr;
    ping_buffer = binder_internal_control_block_mgr::get_instance().get_buffer(
        ping_reply.size() + 1 );
    strncpy_s( ping_buffer, ping_reply.size() + 1, ping_reply.c_str(), ping_reply.size() );
    tr.data.ptr.buffer = reinterpret_cast< binder_uintptr_t >( ping_buffer );
    tr.data_size = ping_reply.size() + 1;
    std::string source_connection_id = ping_msg_reply->get_source_connection_id();
    if( source_connection_id.size() < MAX_CONNECTION_NAME_SIZE )
    {
        strncpy_s( tr.source_connection_name, source_connection_id.c_str(), source_connection_id.size() );
    }
    tr.current_transaction_message_id = ping_msg_reply->get_id();
    parcel_writer->write( &tr, sizeof( binder_transaction_data ) );

    parcel_writer->copy_to( read_ptr, parcel_writer->size() );
    a_wr_blk->read_consumed = parcel_writer->size();
    return 0;
}

int client_control_block::handle_general_transaction
    (
    binder_write_read* a_wr_blk,
    binder_transaction_data& a_write_cur_ptr
    )
{

    int status = 0;

    uint32_t consumed_size = 0;
    consumed_size += sizeof( uint32_t );
    consumed_size += sizeof( binder_transaction_data );
    a_wr_blk->read_consumed = consumed_size; // We wrote consumed_size bytes to read buffer.

    if( !check_connection_ready() )
    {
        LOG( FATAL ) << "before register service, need create client to service manager and ping it";
        return -1;
    }

    std::shared_ptr<data_link::binder_transaction_message> transaction_msg;
    transaction_msg = std::make_shared<data_link::binder_transaction_message>();
    uint64_t msg_id = transaction_msg->get_id();
    transaction_msg->init_transaction_context( &a_write_cur_ptr );
    transaction_msg->m_transaction_state = data_link::binder_transaction_message::transaction_state::init;
    transaction_msg->set_id( msg_id );
    std::string debug_info = android::ipc_connection_token_mgr::get_instance().get_local_connection_name();
    debug_info.append( " init this message." );
    transaction_msg->set_debug_info( debug_info );
    LOG( INFO ) << "Transaction message with service name: " << transaction_msg->m_tr_service_name
        << ", initing connection id: " << transaction_msg->get_source_connection_id() << ", message id: "
        << transaction_msg->get_id() << ", we set state: " <<
        data_link::binder_transaction_message::state_to_string( transaction_msg->m_transaction_state );

    std::shared_ptr<data_link::binder_ipc_message> reply_msg;
    std::string target_connection_name;
    std::string target_service_name;
    int find_ret = android::ipc_connection_token_mgr::get_instance()
        .find_remote_service_by_id( a_write_cur_ptr.target.binder_handle,
                                    target_service_name, target_connection_name );
    if( find_ret != 0 )
    {
        target_connection_name = android::ipc_connection_token_mgr::get_instance()
            .get_current_transaction_connection_name();
        target_service_name = a_write_cur_ptr.service_name;
        if( target_connection_name.empty() && a_write_cur_ptr.target.binder_handle == 0 )
        {
            target_connection_name = ::android::ipc_connection_token_mgr::get_instance()
                .get_service_manager_connection_name();
        }
    }
    transaction_msg->set_target_connection_name( target_connection_name );
    m_client->send_message( transaction_msg );

    uint64_t waiting_id = transaction_msg->get_id();
    LOG( INFO ) << "Transaction ID " << waiting_id << " waiting for handling status from peer.";
    for( ;; wait_for_contion_variable() )
    {
        // 1. check connection if OK or not.
        if( !check_connection_ready() )
        {
            ALOGE( "connection broken when waiting for response" );
            return -1;
        }

        // 2. check incoming message
        reply_msg = check_received_transaction_message( waiting_id, target_service_name,
                                                        target_connection_name,
                                                        a_write_cur_ptr.is_aidl_transaction );

        if( !reply_msg )
        {
            continue;
        }
        break;
    }

    transaction_msg = std::dynamic_pointer_cast< data_link::binder_transaction_message >( reply_msg );
    if( !transaction_msg )
    {
        LOG( ERROR ) << "Not binder_transaction_message!";
        return -1;
    }

    if( transaction_msg->m_transaction_state != data_link::binder_transaction_message::transaction_state::handling )
    {
        LOG( ERROR ) << "We are waiting for our message handling reply. but received a init message.";
    }

    if( a_write_cur_ptr.target.binder_handle == 0 )
    {
        ::android::ipc_connection_token_mgr::get_instance()
            .set_service_manager_connection_name( transaction_msg->get_source_connection_id() );
    }

    uint8_t* read_ptr = reinterpret_cast< uint8_t* >( a_wr_blk->read_buffer );
    if( read_ptr )
    {
        binder_transaction_data tr;
        memset( &tr, 0x00, sizeof( binder_transaction_data ) );
        transaction_msg->parse_transaction_context( &tr );

        std::shared_ptr<android::parcel_writer_interface> parcel_writer;
        parcel_writer = tr.is_aidl_transaction ? android::get_parcel_writer_maker()( )
            : android::get_hidl_parcel_writer_maker()( );
        parcel_writer->write_int32_t( BR_REPLY );
        parcel_writer->write( &tr, sizeof( binder_transaction_data ) );

        parcel_writer->copy_to( read_ptr, parcel_writer->size() );
        a_wr_blk->read_consumed = parcel_writer->size();
    }

    transaction_msg->m_transaction_state = data_link::binder_transaction_message::transaction_state::completed;
    transaction_msg->extract_raw_buffer(); // clear the raw data part.
    LOG( INFO ) << "Message id: " << transaction_msg->get_id() << ". From "
        << transaction_msg->get_source_connection_id() << " we set state: completed";
    transaction_msg->set_debug_info( "set trasaction completed!" );
    transaction_msg->set_target_connection_name( transaction_msg->get_source_connection_id() ); // Since we are goint to reply.
    transaction_msg->set_source_connection_name( android::ipc_connection_token_mgr::get_instance().get_local_connection_name() );
    m_client->send_message( transaction_msg );

    return status;
}

int client_control_block::handle_binder_message_sending
    (
    binder_write_read* a_wr_blk,
    binder_transaction_data& a_write_cur_ptr
    )
{
    int status = 0;
    uint32_t consumed_size = 0;
    consumed_size += sizeof( uint32_t );
    consumed_size += sizeof( binder_transaction_data );
    a_wr_blk->read_consumed = consumed_size; // We wrote consumed_size bytes to read buffer.

    std::shared_ptr<data_link::binder_transaction_message> send_msg;
    send_msg = std::make_shared<data_link::binder_transaction_message>();
    send_msg->init_transaction_context( &a_write_cur_ptr );

    if( m_client->get_connection_status() == data_link::connection_status::connected )
    {
        m_client->send_message( send_msg );
    }
    else
    {
        LOG( ERROR ) << "Not connected. message missed.";
    }

    return status;
}

void client_control_block::wait_for_contion_variable()
{
    std::chrono::seconds wait_time{ 1 };
    std::unique_lock<std::recursive_mutex> lcker( m_mutex );
    m_condition_var.wait_for( lcker, wait_time );
}

bool client_control_block::check_connection_ready()
{
    bool connection_ready = false;
    if( m_client )
    {
        auto status = m_client->get_connection_status();
        if( status == data_link::connection_status::connected )
        {
            return true;
        }
    }
    else
    {
        LOG( ERROR ) << "Should not running here.";
    }

    if( data_link::connection_status::disconnected ==
        m_client->get_connection_status() )
    {
        std::string service_addr;
        if( m_service_id == 0 )
        {
            service_addr = binder_internal_control_block_mgr::get_instance()
                .get_service_manager_endpoint();
        }
        else
        {
            service_addr = android::ipc_connection_token_mgr::get_instance()
                .find_remote_service_listen_addr( m_service_id );
        }

        LOG( INFO ) << "try to connect " << service_addr;
        m_client->connect( service_addr );
    }

    return connection_ready;
}

std::shared_ptr<data_link::binder_ipc_message>
    client_control_block::check_receivd_message
    (
    std::string const& a_connection_name,
    uint64_t a_message_id,
    data_link::binder_message_type a_msg_type,
    bool a_is_hidl_message
    )
{
    std::lock_guard<std::recursive_mutex> lcker( m_mutex );
    if( a_is_hidl_message )
    {
        if( !m_received_binder_messages.empty() )
        {
            if( m_received_binder_messages.front()->get_type() == a_msg_type )
            {
                auto ret = std::move( m_received_binder_messages.front() );
                m_received_binder_messages.erase( m_received_binder_messages.begin() );
                return ret;
            }
        }
    }
    else
    {
        if( !m_received_hidl_messages.empty() )
        {
            if( m_received_hidl_messages.front()->get_type() == a_msg_type )
            {
                auto ret = std::move( m_received_hidl_messages.front() );
                m_received_hidl_messages.erase( m_received_hidl_messages.begin() );
                return ret;
            }
        }
    }

    return nullptr;
}

std::shared_ptr<data_link::binder_transaction_message>
    client_control_block::check_received_transaction_message
    (
    uint64_t a_id,
    std::string const& a_service_name,
    std::string const& a_source_connection_name,
    bool a_aidl_message_checking
    )
{
    bool nested_transaction = false;
    std::shared_ptr<data_link::binder_transaction_message> ret_detail;
    std::unique_lock<std::recursive_mutex> lcker( m_mutex );
    std::vector<std::shared_ptr<data_link::binder_ipc_message>>* received_binder_message = nullptr;
    received_binder_message = a_aidl_message_checking ? &m_received_binder_messages : &m_received_hidl_messages;
    for( auto it = received_binder_message->begin(); it != received_binder_message->end(); )
    {
        auto ele = *it;
        if( !ele )
        {
            it = received_binder_message->erase( it );
            continue;
        }

        if( ele->get_type() != data_link::binder_message_type::binder_transaction )
        {
            ++it;
            continue;
        }

        ret_detail = std::dynamic_pointer_cast<data_link::binder_transaction_message>( ele );
        if( !ret_detail )
        {
            ++it;
            continue;
        }

        int str_cmp_result = std::strcmp( ret_detail->get_source_connection_id().c_str(), a_source_connection_name.c_str() );

        if( ( str_cmp_result != 0 ) &&
            !a_source_connection_name.empty() )
        {
            ALOGI( "received transaction message with not expected connection id, check next one" );
            ++it;
            ret_detail.reset();
            continue;
        }

        if( ret_detail->get_id() != a_id )
        {
            ALOGI( "received transaction message with not expected id, check next one" );
            ++it;
            ret_detail.reset();
            nested_transaction = true;
            continue;
        }

        str_cmp_result = std::strcmp( ret_detail->m_tr_service_name.c_str(), a_service_name.c_str() );
        if( str_cmp_result != 0 )
        {
            ALOGI( "received transaction message with not expected service name, check next one" );
            ++it;
            ret_detail.reset();
            continue;
        }

        it = received_binder_message->erase( it );
        break;
    }

    lcker.unlock();
    if( !ret_detail && nested_transaction )
    {
        ALOGI( "We are in one transaction but peer side init new transaction." );
        binder_internal_control_block_mgr::get_instance().invoke_hidl_data_handler();
    }

    return ret_detail;
}

void client_control_block::handle_incoming_ipc_message
    (
    uint32_t a_binder_fd_handle,
    binder_write_read* a_wr_blk
    )
{
    std::shared_ptr<data_link::binder_ipc_message> msg;
    std::unique_lock<std::recursive_mutex> lcker( m_mutex );
    if( a_binder_fd_handle == AIDL_BINDER_FD )
    {
        if( m_received_binder_messages.empty() )
        {
            LOG( INFO ) << "No received binder message";
            return;
        }

        msg = m_received_binder_messages.front();
        m_received_binder_messages.erase( m_received_binder_messages.begin() );
    }
    else if( a_binder_fd_handle == HIDL_BINDER_FD )
    {
        if( m_received_hidl_messages.empty() )
        {
            LOG( INFO ) << "No received binder message";
            return;
        }

        msg = m_received_hidl_messages.front();
        m_received_hidl_messages.erase( m_received_hidl_messages.begin() );
    }
    lcker.unlock();

    if( !msg )
    {
        LOG( ERROR ) << "Empty binder message.";
        return;
    }

    if( m_remote_connection_name.empty() )
    {
        m_remote_connection_name = msg->get_source_connection_id();
        if( m_remote_connection_name.empty() )
        {
            LOG( ERROR ) << "remote side need contain connection name!";
            return;
        }
    }
    else
    {
        if( m_remote_connection_name != msg->get_source_connection_id() )
        {
            LOG( ERROR ) << "m_remote_connection_name: " << m_remote_connection_name
                << ", message's connection name: " << msg->get_source_connection_id()
                << ", not same!";
            return;
        }
    }

    switch( msg->get_type() )
    {
    case data_link::binder_message_type::ping_service_manager:
        handle_ping_message( std::dynamic_pointer_cast< data_link::binder_ping_message >( msg ) );
        break;
    case data_link::binder_message_type::register_service:
        //handle_register_service_message( std::dynamic_pointer_cast< data_link::register_service_message >( msg ) );
        LOG( ERROR ) << "need handle this message. register_service";
        break;
    case data_link::binder_message_type::binder_transaction:
        handle_general_transaction_message( a_binder_fd_handle, a_wr_blk,
                                            std::dynamic_pointer_cast<data_link::binder_transaction_message>( msg ) );
        break;
    default:
        ALOGE( "unknown message type!" );
        break;
    }
}

uint32_t client_control_block::get_incoming_message_size( uint32_t a_binder_fd_handle )
{
    std::lock_guard<std::recursive_mutex> lcker( m_mutex );
    if( a_binder_fd_handle == AIDL_BINDER_FD )
    {
        return m_received_binder_messages.size();
    }
    else if( a_binder_fd_handle == HIDL_BINDER_FD )
    {
        return m_received_hidl_messages.size();
    }
}

void client_control_block::receive_incoming_ipc_message
    (
    std::shared_ptr<data_link::binder_ipc_message> a_msg
    )
{
    LOG(INFO) << "Received message id: " << a_msg->get_id();
    std::unique_lock<std::recursive_mutex> lcker( m_mutex );
    if( a_msg->is_aidl_message() )
    {
        m_received_binder_messages.push_back( a_msg );
    }
    else
    {
        m_received_hidl_messages.push_back( a_msg );
    }
    lcker.unlock();
    m_condition_var.notify_all();

    std::this_thread::yield();

    if( a_msg->is_aidl_message() )
    {
        binder_internal_control_block_mgr::get_instance().invoke_binder_data_handler();
    }
    else
    {
        binder_internal_control_block_mgr::get_instance().invoke_hidl_data_handler();
    }
}

void client_control_block::send_reply_only
    (
    binder_write_read* a_wr_blk,
    binder_transaction_data& a_write_cur_ptr
    )
{
    uint32_t binder_fd_handle = a_write_cur_ptr.is_aidl_transaction ? AIDL_BINDER_FD : HIDL_BINDER_FD;
    auto msg = get_processing_transaction
        (
        a_write_cur_ptr.current_transaction_message_id,
        a_write_cur_ptr.service_name,
        a_write_cur_ptr.source_connection_name
        );
    if( !msg )
    {
        LOG( ERROR ) << "no message waiting for reply.";
        return;
    }
    msg->init_transaction_context( &a_write_cur_ptr );
    msg->m_is_reply = true;
    msg->set_from_client( !binder_internal_control_block_mgr::get_instance().is_service_manager() );
    msg->set_target_connection_name( msg->get_source_connection_id() ); // Since we are goint to reply.
    msg->set_source_connection_name( android::ipc_connection_token_mgr::get_instance().get_local_connection_name());

    m_client->send_message( msg );
    LOG( INFO ) << "Transaction message with service name: " << msg->m_tr_service_name
        << ", connection id: " << msg->get_source_connection_id() << ", message id: "
        << msg->get_id() << ", state: " << data_link::binder_transaction_message::state_to_string
            ( msg->m_transaction_state ) << ". Waiting for completed reply.";

    std::shared_ptr<data_link::binder_transaction_message> reply_msg;
    std::chrono::steady_clock::time_point start_ = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end_ = start_;
    for( ;; wait_for_contion_variable() )
    {
        // 1. check connection if OK or not.
        if( !check_connection_ready() )
        {
            ALOGE( "connection broken when waiting for response" );
            return;
        }

        // 2. check incoming message
        reply_msg = check_received_transaction_message( msg->get_id(), msg->m_tr_service_name,
                                                        msg->get_target_connection_id(),
                                                        a_write_cur_ptr.is_aidl_transaction );
        if( !reply_msg )
        {
            end_ = std::chrono::steady_clock::now();
            if( end_ - start_ > std::chrono::seconds( 10 ) )
            {
                start_ = end_;
                ALOGE( "Wait for to much time. Should take" );
            }
            continue;
        }

        break;
    }

    handle_general_transaction_message( binder_fd_handle, a_wr_blk, reply_msg );
}

void client_control_block::handle_connection_status_changed( data_link::connection_status a_status )
{
    if( m_connection_notify_internal )
    {
        m_connection_notify_internal( a_status );
    }

    binder_internal_control_block_mgr::get_instance().handle_client_status_changed( this, a_status );
}

void client_control_block::handle_ping_message
    (
    std::shared_ptr<data_link::binder_ping_message> a_message
    )
{
    if( !a_message )
    {
        ALOGE( "null ping message" );
        return;
    }

    ALOGI( "Receive ping from %s: %s", a_message->get_endpoint().c_str(),
        a_message->m_ping_message.c_str() );
    std::shared_ptr<data_link::binder_ping_message> reply;
    reply = a_message;
    reply->set_from_client( !binder_internal_control_block_mgr::get_instance().is_service_manager() );
    reply->m_ping_message = "service manager ping back";
    reply->extract_raw_buffer();
    reply->set_id( a_message->get_id() );
    reply->set_endpoint( a_message->get_endpoint() );
    m_client->send_message( reply );
}

void client_control_block::handle_general_transaction_message
    (
    uint32_t a_binder_fd_handle,
    binder_write_read* a_wr_blk,
    std::shared_ptr<data_link::binder_transaction_message> a_message
    )
{
    uint8_t* read_ptr = reinterpret_cast< uint8_t* >( a_wr_blk->read_buffer );
    if( !read_ptr )
    {
        LOG( FATAL ) << "read_ptr is nullptr!";
        return;
    }

    LOG( INFO ) << "Transaction message with service name: " << a_message->m_tr_service_name
        << ", connection id: " << a_message->get_source_connection_id() << ", message id: "
        << a_message->get_id() << ", state: " <<
        data_link::binder_transaction_message::state_to_string( a_message->m_transaction_state );

    std::shared_ptr<android::parcel_writer_interface> parcel_writer;
    if( a_binder_fd_handle == AIDL_BINDER_FD )
    {
        parcel_writer = android::get_parcel_writer_maker()();
    }
    else if( a_binder_fd_handle == HIDL_BINDER_FD )
    {
        parcel_writer = android::get_hidl_parcel_writer_maker()();
    }
    else
    {
        LOG( ERROR ) << "Unknown binder type.";
        return;
    }

    if( a_message->m_transaction_state == data_link::binder_transaction_message::transaction_state::completed )
    {
        parcel_writer->writeUint32( BR_TRANSACTION_COMPLETE );
    }
    else
    {
        parcel_writer->writeUint32( BR_TRANSACTION );
    }

    binder_transaction_data tr;
    memset( &tr, 0x00, sizeof( binder_transaction_data ) );
    a_message->parse_transaction_context( &tr);
    tr.current_transaction_message_id = a_message->get_id();
    parcel_writer->write( &tr, sizeof( binder_transaction_data ) );

    parcel_writer->copy_to( read_ptr, parcel_writer->size() );
    a_wr_blk->read_consumed = parcel_writer->size();
    if( a_message->m_transaction_state == data_link::binder_transaction_message::transaction_state::init )
    {
        a_message->m_transaction_state = data_link::binder_transaction_message::transaction_state::handling;
        LOG( INFO ) << "Transaction message with service name: " << a_message->m_tr_service_name
            << ", connection id: " << a_message->get_source_connection_id() << ", message id: "
            << a_message->get_id() << ", we set state: " <<
                data_link::binder_transaction_message::state_to_string( a_message->m_transaction_state );
        m_transaction_in_process_msgs.push_back( a_message );
    }
}

void client_control_block::init_client_to_service()
{
    if( m_client )
    {
        return;
    }

    m_client = data_link::client::make_one();
    m_client->register_message_incoming_callback
        (
        std::bind( &client_control_block::receive_incoming_ipc_message,
            this, std::placeholders::_1 )
        );
    m_client->register_connection_status_callback( std::bind(
        &client_control_block::handle_connection_status_changed, this, std::placeholders::_1 ) );

    std::string listen_addr;
    if( m_service_id == 0 )
    {
        listen_addr = binder_internal_control_block_mgr::get_instance()
            .get_service_manager_endpoint();
    }
    else
    {
        std::string service_name;
        std::string connection_name;
        android::ipc_connection_token_mgr::get_instance().find_remote_service_by_id
            ( m_service_id, service_name, connection_name );
        listen_addr = android::ipc_connection_token_mgr::get_instance()
            .find_remote_service_listen_addr( m_service_id );
        m_remote_connection_name = connection_name;
    }

    if( listen_addr.empty() )
    {
        LOG( ERROR ) << "No such remote service with id: " << m_service_id;
        return;
    }

    std::promise<void> _promise;
    std::future<void> _future = _promise.get_future();
    auto fun = [&_promise]( data_link::connection_status a_status )
    {
        if( a_status == data_link::connection_status::connected )
        {
            _promise.set_value();
        }
    };
    m_connection_notify_internal = fun;
    LOG( INFO ) << "Try to connect: " << listen_addr;
    m_client->connect( listen_addr );
    _future.wait();
    m_connection_notify_internal = nullptr;
    LOG( INFO ) << "Connected to " << listen_addr;
    m_remote_address = listen_addr;
}

std::shared_ptr<data_link::binder_transaction_message>
    client_control_block::get_processing_transaction
    (
    uint64_t a_message_id,
    std::string a_service_name,
    std::string a_source_connection_name
    )
{
    for( auto it = m_transaction_in_process_msgs.begin(); it != m_transaction_in_process_msgs.end(); ++it )
    {
        auto ele = *it;
        if( ele->get_id() == a_message_id &&
            ele->m_tr_service_name == a_service_name &&
            ele->get_source_connection_id() == a_source_connection_name )
        {
            it = m_transaction_in_process_msgs.erase( it );
            return ele;
        }
    }
    return nullptr;
}

