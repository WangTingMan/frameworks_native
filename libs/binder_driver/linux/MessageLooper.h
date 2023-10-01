#pragma once

#include <functional>
#include <map>
#include <memory>

#include <linux/libbinder_driver_exports.h>

class MessageLooperControlBlock;

class LIBBINDERDRIVER_EXPORTS MessageLooper
{

public:

    MessageLooper();

    static MessageLooper& GetDefault();

    void Run();

    int RegisterTimer
        (
        int a_milliseconds,
        std::function<bool( void )> a_fun
        );

    void PostTask( std::function<void()> a_task );

    void PostDelayTask
        (
        int a_milliseconds,
        std::function<void()> a_task
        );

    bool IsLooperThread();

private:

    void timer_function_wrapper( int index, std::function<bool( void )> a_fun );

    std::shared_ptr<MessageLooperControlBlock> m_control_block;
};

