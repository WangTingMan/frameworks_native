#include <linux/binder_internal_control_block_mgr.h>
#include <linux/binder_internal_control_block.h>
#include <base/logging.h>
#include <parcel_porting.h>

std::atomic_uint32_t binder_internal_control_block_mgr::m_next_id = 5;

binder_internal_control_block_mgr& binder_internal_control_block_mgr::get_instance()
{
    static binder_internal_control_block_mgr instance;
    return instance;
}

std::shared_ptr<binder_internal_control_block> binder_internal_control_block_mgr::find_by_id( uint32_t a_id )
{
    std::shared_lock<std::shared_mutex> locker( m_mutex );
    for( auto& ele : m_blocks )
    {
        if( ele->get_id() == a_id )
        {
            return ele;
        }
    }

    return nullptr;
}

std::shared_ptr<binder_internal_control_block> binder_internal_control_block_mgr::create_new_one()
{
    auto ret = std::make_shared<binder_internal_control_block>();
    ret->id = m_next_id++;

    std::lock_guard<std::shared_mutex> locker( m_mutex );
    m_blocks.push_back( ret );
    return ret;
}

bool binder_internal_control_block_mgr::remove_control_block( uint32_t a_id )
{
    std::lock_guard<std::shared_mutex> locker( m_mutex );
    for( auto it = m_blocks.begin(); it != m_blocks.end(); )
    {
        if( ( *it )->get_id() == a_id )
        {
            it = m_blocks.erase( it );
            return true;
        }
        else
        {
            ++it;
        }
    }
    return false;
}

void binder_internal_control_block_mgr::enter_looper()
{
    LOG( INFO ) << "enter looper for incoming message.";
    bool running_as_server = false;
    std::lock_guard<std::shared_mutex> locker( m_mutex );

    running_as_server = m_is_service_manager;

    if( !running_as_server )
    {
        LOG( WARNING ) << "since not server. so exit.";
        return;
    }

    if( !m_server )
    {
        m_server = data_link::server::make_one();
        m_server->register_message_incoming_callback(
            std::bind( &binder_internal_control_block_mgr::handle_incoming_message,
                this, std::placeholders::_1 ) );
    }
    else
    {
        ALOGW( "Already created server instance." );
    }

    std::string server_endpoint;
    if( m_is_service_manager )
    {
        server_endpoint = get_service_manager_endpoint();
        ALOGI( "start running as binder service manager." );
    }

    m_server->listen_on( server_endpoint, running_as_server );
}

bool binder_internal_control_block_mgr::register_service_local
    (
    std::shared_ptr<android::AddServiceControlBlock> a_service
    )
{
    bool ret = false;
    std::lock_guard<std::shared_mutex> lcker( m_mutex );

    for( auto ele : mAddedServices )
    {
        if( ele->service_name == a_service->service_name )
        {
            ret = false;
            ALOGE( "%s already registered!", ele->service_name.c_str() );
            return ret;
        }
    }

    mAddedServices.emplace_back( std::move( a_service ) );
    ret = true;
    return ret;
}

char* binder_internal_control_block_mgr::get_buffer( uint32_t a_size_expect )
{
    std::lock_guard<std::shared_mutex> locker( m_mutex );
    for( auto it = m_free_buffers.begin(); it != m_free_buffers.end(); ++it )
    {
        auto& ele = *it;
        if( ele->capacity() >= a_size_expect )
        {
            char* buffer = ele->data();
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
    std::lock_guard<std::shared_mutex> locker( m_mutex );
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

void binder_internal_control_block_mgr::handle_incoming_message
    (
    std::shared_ptr<data_link::binder_ipc_message> a_message
    )
{
    switch( a_message->get_type() )
    {
    case data_link::binder_message_type::ping_service_manager:
        handle_ping_message( std::dynamic_pointer_cast<data_link::binder_ping_message>( a_message ) );
        break;
    case data_link::binder_message_type::register_service:
        handle_register_service_message( std::dynamic_pointer_cast< data_link::register_service_message >( a_message ) );
        break;
    default:
        ALOGE( "unknown message type!" );
        break;
    } 
}

void binder_internal_control_block_mgr::handle_ping_message
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
    a_message->set_from_client( !m_is_service_manager );
    a_message->m_ping_message = "service manager ping back";
    a_message->extract_raw_buffer();
    m_server->send_message( a_message );
}

void binder_internal_control_block_mgr::handle_register_service_message
    (
    std::shared_ptr<data_link::register_service_message> a_message
    )
{
    LOG(INFO) << "register service. name: " << a_message->m_service_name
        << ", from: " << a_message->get_endpoint();

    a_message->set_from_client( !m_is_service_manager );
    a_message->m_register_result = 0;
    a_message->extract_raw_buffer();
    m_server->send_message( a_message );
}
