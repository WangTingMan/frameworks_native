
#include <linux/MessageLooper.h>
#include <base\system_monitor\system_monitor.h>
#include <base\message_loop\message_loop.h>
#include <base\observer_list_threadsafe.h>
#include <base\memory\scoped_refptr.h>
#include <base\time\time.h>
#include <base\timer\timer.h>
#include <base\threading\platform_thread.h>

#include <atomic>

class MessageLooperControlBlock
{

public:

    std::atomic_uint64_t _running_thread_id = 0;
    base::MessageLoop _msgLoop;
    base::RunLoop _loop;
    std::map<int, std::shared_ptr<base::RepeatingTimer>> _timers;
    int _current_timer_index = 0;
};

static void ___task_wrapper( std::function<void()> a_task )
{
    a_task();
}

MessageLooper& MessageLooper::GetDefault()
{
    static MessageLooper instance;
    return instance;
}

MessageLooper::MessageLooper()
{
    m_control_block = std::make_shared<MessageLooperControlBlock>();
}

void MessageLooper::Run()
{
    base::PlatformThreadId running_thread = base::PlatformThread::CurrentId();
    m_control_block->_running_thread_id.exchange( running_thread );
    m_control_block->_loop.Run();
}

int MessageLooper::RegisterTimer
    (
    int a_milliseconds,
    std::function<bool( void )> a_fun
    )
{
    int index = m_control_block->_current_timer_index++;
    std::shared_ptr<base::RepeatingTimer> timer = std::make_shared<base::RepeatingTimer>();
    m_control_block->_timers[index] = timer;

    timer->Start( FROM_HERE, base::TimeDelta::FromMilliseconds( a_milliseconds ),
        base::Bind( &MessageLooper::timer_function_wrapper, base::Unretained( this ), index, a_fun ) );

    return index;
}

void MessageLooper::PostTask( std::function<void()> a_task )
{
    m_control_block->_msgLoop.task_runner()->PostTask( FROM_HERE, base::Bind( &___task_wrapper, a_task ) );
}

void MessageLooper::PostDelayTask
    (
    int a_milliseconds,
    std::function<void()> a_task
    )
{
    base::TimeDelta delay = base::TimeDelta::FromMilliseconds( a_milliseconds );
    m_control_block->_msgLoop.task_runner()->PostDelayedTask( FROM_HERE, base::Bind( &___task_wrapper, a_task ), delay );
}

bool MessageLooper::IsLooperThread()
{
    base::PlatformThreadId running_thread = base::PlatformThread::CurrentId();
    return running_thread == m_control_block->_running_thread_id;
}

void MessageLooper::timer_function_wrapper( int index, std::function<bool( void )> a_fun )
{
    bool ret = a_fun();
    if( ret )
    {
        auto it = m_control_block->_timers.find( index );
        if( it == m_control_block->_timers.end() )
        {
            return;
        }

        it->second->Stop();

        m_control_block->_timers.erase( it );
    }
}
