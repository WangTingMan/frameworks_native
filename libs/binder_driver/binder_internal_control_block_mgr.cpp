#include <linux/binder_internal_control_block_mgr.h>
#include "internal/client_control_block.h"

#include <base/logging.h>
#include <ipc_connection_token.h>
#include <binder_utils.h>
#include <data_link/message_types.h>
#include <memory>
#include <functional>
#include <future>

binder_internal_control_block_mgr& binder_internal_control_block_mgr::get_instance()
{
    static binder_internal_control_block_mgr instance;
    return instance;
}

void binder_internal_control_block_mgr::enter_looper()
{
    std::lock_guard<std::recursive_mutex> locker( m_mutex );
    if( m_is_service_manager )
    {
        LOG( INFO ) << "start run service manager.";
        start_local_link_server();
    }
}

int binder_internal_control_block_mgr::handle_read_only( binder_write_read* a_wr_blk )
{
    int status = 0;
    uint8_t* read_ptr = reinterpret_cast<uint8_t*>( a_wr_blk->read_buffer );
    if( !read_ptr )
    {
        LOG( FATAL ) << "read_ptr is nullptr!";
        return status;
    }

    std::unique_lock<std::recursive_mutex> lcker( m_mutex );
    for( auto& ele : m_clients )
    {
        if( ele->get_incoming_message_size() > 0 )
        {
            ele->handle_incoming_ipc_message( a_wr_blk );
            break;
        }
    }

    return status;
}

char* binder_internal_control_block_mgr::get_buffer( uint32_t a_size_expect )
{
    std::lock_guard<std::recursive_mutex> locker( m_mutex );
    for( auto it = m_free_buffers.begin(); it != m_free_buffers.end(); ++it )
    {
        auto& ele = *it;
        if( ele->capacity() >= a_size_expect )
        {
            char* buffer = ele->data();
            memset( buffer, 0x00, ele->capacity() );
            m_used_buffers.insert( ele );
            m_free_buffers.erase( it );
            return buffer;
        }
    }

    uint32_t mul_ = 1 + a_size_expect / 10;
    a_size_expect = mul_ * 10;

    std::shared_ptr<std::vector<char>> new_buffer;
    new_buffer = std::make_shared<std::vector<char>>();
    new_buffer->resize( a_size_expect );
    m_used_buffers.insert( new_buffer );
    return new_buffer->data();
}

void binder_internal_control_block_mgr::return_back_buffer( char* a_buffer )
{
    bool found = false;
    std::lock_guard<std::recursive_mutex> locker( m_mutex );
    for( auto it = m_used_buffers.begin(); it != m_used_buffers.end();)
    {
        auto& ele = *it;
        if( ele->data() == a_buffer )
        {
            m_free_buffers.push_back( ele );
            it = m_used_buffers.erase( it );
            found = true;
            return;
        }
        else
        {
            ++it;
        }
    }

    if( !found )
    {
        ALOGE( "Not found such pointer: %X", a_buffer );
    }
}

void binder_internal_control_block_mgr::start_local_link_server()
{
    if( !m_server )
    {
        m_server = data_link::server::make_one();
        m_server->new_client_connected( std::bind( 
            &binder_internal_control_block_mgr::handle_new_client_incoming,
            this, std::placeholders::_1 ) );
    }
    else
    {
        ALOGE( "Already created server instance." );
        return;
    }

    std::string server_endpoint;
    if( m_is_service_manager )
    {
        server_endpoint = get_service_manager_endpoint();
        LOG(INFO) << "start running as binder service manager. listen on: " << server_endpoint;
    }
    else
    {
        server_endpoint = android::ipc_connection_token_mgr::get_instance().get_local_listen_address();
        LOG(INFO) << "start running as normal binder service. listen on: " << server_endpoint;
    }

    m_server->listen_on( server_endpoint, false );
}

void binder_internal_control_block_mgr::invoke_binder_data_handler()
{
    std::unique_lock<std::recursive_mutex> lcker( m_mutex );
    auto handler = m_binder_data_handler;
    lcker.unlock();
    if( handler )
    {
        handler();
    }
}

std::shared_ptr<data_link::binder_ipc_message> binder_internal_control_block_mgr::get_previous_handle_message()
{
    return m_previous_handles_message;
}

int binder_internal_control_block_mgr::handle_write_read_block( binder_write_read* a_wr_blk )
{
    int status = 0;
    uint8_t* ptr = reinterpret_cast< uint8_t* >( a_wr_blk->write_buffer ) + a_wr_blk->write_consumed;
    uint32_t cmd = 0;

    if( a_wr_blk->write_size == 0 )
    {
        return handle_read_only( a_wr_blk );
    }

    std::shared_ptr<android::parcel_writer_interface> viewer_mOut;
    viewer_mOut = android::get_parcel_writer_maker()( );
    viewer_mOut->ipcSetDataReference( ptr, a_wr_blk->write_size, nullptr, 0 );
    cmd = viewer_mOut->readInt32();

    switch( cmd )
    {
    case BC_INCREFS:
    {
        int32_t handle = 0;
        memcpy( &handle, ptr, sizeof( int32_t ) );
        a_wr_blk->write_consumed = sizeof( cmd ) + sizeof( handle );
    }
    break;
    case BC_ACQUIRE:
    {
        int32_t handle = 0;
        memcpy( &handle, ptr, sizeof( int32_t ) );
        a_wr_blk->write_consumed = sizeof( cmd ) + sizeof( handle );
    }
    break;
    case BC_RELEASE:
    {
        int32_t handle = 0;
        handle = viewer_mOut->readInt32();
        a_wr_blk->write_consumed = sizeof( cmd ) + sizeof( handle );
        // since we are not use real linux binder, so we just ignore this cmd.
    }
        break;
    case BC_TRANSACTION:
    {
        binder_transaction_data tr;
        viewer_mOut->read( ( void* )( &tr ), sizeof( binder_transaction_data ) );
        auto client = find_client( tr.target.binder_handle );
        if( !client )
        {
            std::string listen_addr;
            listen_addr = android::ipc_connection_token_mgr::get_instance()
                .find_remote_service_listen_addr( tr.target.binder_handle );
            client = find_client( listen_addr );
        }

        if( !client )
        {
            LOG( INFO ) << "make client to binder handle: " << tr.target.binder_handle;
            client = std::make_shared<client_control_block>( tr.target.binder_handle );
            std::lock_guard<std::recursive_mutex> locker( m_mutex );
            m_clients.push_back( client );
        }
        client->handle_transaction( a_wr_blk, tr );

        a_wr_blk->write_consumed = a_wr_blk->write_size;
        break;
    }
    case BC_REPLY:
    {
        binder_transaction_data tr;
        viewer_mOut->read( ( void* )( &tr ), sizeof( binder_transaction_data ) );
        bool is_valid_reply = false;
        if( tr.target.binder_handle == -1 &&
            tr.code == 0 )
        {
            is_valid_reply = true;
        }
        else
        {
            LOG( ERROR ) << "Not valid reply";
        }


        std::vector<std::shared_ptr<client_control_block>> reply_clients;
        if( is_valid_reply )
        {
            std::lock_guard<std::recursive_mutex> locker( m_mutex );
            for( auto& ele : m_clients )
            {
                if( ele->has_trasaction_need_reply() )
                {
                    reply_clients.push_back( ele );
                }
            }
        }

        if( reply_clients.size() > 1 )
        {
            LOG( ERROR ) << "Got more than 1 client need reply!";
        }

        for( auto& ele : reply_clients )
        {
            ele->send_reply_only( a_wr_blk, tr );
        }
        a_wr_blk->write_consumed = a_wr_blk->write_size;
        break;
    }
    case BC_FREE_BUFFER:
    {
        char* buffer = ( char* )viewer_mOut->readUint64();
        return_back_buffer( buffer );
        a_wr_blk->write_consumed = a_wr_blk->write_size;
    }
    break;
    case BC_ENTER_LOOPER:
        a_wr_blk->write_consumed = sizeof( cmd );
        enter_looper();
        break;
    case BC_REQUEST_DEATH_NOTIFICATION:
        a_wr_blk->write_consumed = a_wr_blk->write_size;
        LOG( INFO ) << "TODO add death notification implementation.";
        break;
    default:
        LOG( ERROR ) << "No implementation with " << cmd;
    }

    return status;
}

std::shared_ptr<client_control_block> binder_internal_control_block_mgr::find_client( uint32_t a_handle )
{
    std::lock_guard<std::recursive_mutex> locker( m_mutex );
    for( auto& ele : m_clients )
    {
        if( ele->get_service_id() == a_handle )
        {
            return ele;
        }
    }
    return nullptr;
}

std::shared_ptr<client_control_block> binder_internal_control_block_mgr::find_client( std::string const& a_listen_addr )
{
    std::lock_guard<std::recursive_mutex> locker( m_mutex );
    for( auto& ele : m_clients )
    {
        if( ele->remote_listen_address() == a_listen_addr )
        {
            return ele;
        }
    }
    return nullptr;
}

void binder_internal_control_block_mgr::handle_new_client_incoming( std::shared_ptr<data_link::client> a_client )
{
    std::shared_ptr<client_control_block> client;
    client = std::make_shared<client_control_block>( a_client );

    std::lock_guard<std::recursive_mutex> lcker( m_mutex );
    m_clients.push_back( client );
}
