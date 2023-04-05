#include <linux/binder_internal_control_block.h>

std::atomic_uint32_t binder_internal_control_block_mgr::m_next_id = 5;

binder_internal_control_block_mgr& binder_internal_control_block_mgr::get_instance()
{
    static binder_internal_control_block_mgr instance;
    return instance;
}

std::shared_ptr<binder_internal_control_block> binder_internal_control_block_mgr::find_by_id(uint32_t a_id)
{
    std::shared_lock<std::shared_mutex> locker(m_mutex);
    for (auto& ele : m_blocks)
    {
        if (ele->get_id() == a_id)
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

    std::lock_guard<std::shared_mutex> locker(m_mutex);
    m_blocks.push_back(ret);
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


