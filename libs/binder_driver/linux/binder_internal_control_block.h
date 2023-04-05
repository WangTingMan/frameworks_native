#pragma once
#include <atomic>
#include <memory>
#include <cstdint>
#include <vector>
#include <shared_mutex>

#include <linux/libbinder_driver_exports.h>

class binder_internal_control_block
{

public:

    friend class binder_internal_control_block_mgr;

    void set_oneway_spam_detection_enabled(bool a_enable = true)
    {
        oneway_spam_detection_enabled = a_enable;
    }

    uint32_t get_id()const
    {
        return id;
    }

private:

    bool oneway_spam_detection_enabled = false;
    uint32_t id = 0;
};

class LIBBINDERDRIVER_EXPORTS binder_internal_control_block_mgr
{

public:

    static binder_internal_control_block_mgr& get_instance();

    std::shared_ptr<binder_internal_control_block> find_by_id(uint32_t a_id);

    std::shared_ptr<binder_internal_control_block> create_new_one();

    bool remove_control_block( uint32_t a_id );

private:

    static std::atomic_uint32_t m_next_id;

    std::shared_mutex m_mutex;
    std::vector<std::shared_ptr<binder_internal_control_block>> m_blocks;
};

