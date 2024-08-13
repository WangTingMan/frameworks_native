
#include <linux/MessageLooper.h>
#include <base\system_monitor\system_monitor.h>
#include <base\message_loop\message_loop.h>
#include <base\observer_list_threadsafe.h>
#include <base\memory\scoped_refptr.h>
#include <base\time\time.h>
#include <base\timer\timer.h>
#include <base\threading\platform_thread.h>

#include <atomic>
#include <mutex>
#include <thread>

struct PendingTimerBlock
{
    int m_index = 0;
    int m_interval_milliseconds = 0;
    std::function<bool( void )> m_callback;
};

class MessageLooperControlBlock
{

public:

    int _current_timer_index = 0;
    std::vector<std::function<void()>> m_exit_task;
    std::vector<std::function<void()>> m_pending_task;
    std::vector<PendingTimerBlock> m_timer_to_register;
};

class RunningControlBlock
{

public:

    std::atomic_uint64_t _running_thread_id = 0;
    base::MessageLoop _msgLoop;
    base::RunLoop _loop;
    std::map<int, std::shared_ptr<base::RepeatingTimer>> _timers;
    std::atomic_bool m_is_running = false;
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

bool MessageLooper::IsRunning()const
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    return m_running_control_block && m_running_control_block->m_is_running;
}

void MessageLooper::Run()
{
    if( IsRunning() )
    {
        LOG( INFO ) << "Already running.";
        return;
    }

    std::vector<std::function<void()>> pending_task;
    std::unique_lock<std::shared_mutex> lcker( m_mutex );
    m_running_control_block = std::make_shared<RunningControlBlock>();
    base::PlatformThread::SetName( "MessageLooper::Run" );
    m_running_control_block->m_is_running.exchange( true );
    base::PlatformThreadId running_thread = base::PlatformThread::CurrentId();
    m_running_control_block->_running_thread_id.exchange( running_thread );
    for( auto& ele : m_control_block->m_timer_to_register )
    {
        std::shared_ptr<base::RepeatingTimer> timer = std::make_shared<base::RepeatingTimer>();
        m_running_control_block->_timers[ele.m_index] = timer;
        timer->Start( FROM_HERE, base::TimeDelta::FromMilliseconds( ele.m_interval_milliseconds ),
                      base::Bind( &MessageLooper::timer_function_wrapper,
                                  base::Unretained( this ), ele.m_index, ele.m_callback ) );
    }
    m_control_block->m_timer_to_register.clear();
    pending_task = std::move( m_control_block->m_pending_task );
    lcker.unlock();

    for( auto& ele : pending_task )
    {
        ele();
    }

    m_running_control_block->_loop.Run();
    m_running_control_block->m_is_running.exchange( false );

    for( auto& ele : m_control_block->m_exit_task )
    {
        ele();
    }

    lcker.lock();
    m_running_control_block.reset();
    m_quit_promise.set_value();
}

void MessageLooper::Quit()
{
    if (m_running_control_block)
    {
        m_quit_future = m_quit_promise.get_future();
        m_running_control_block->_loop.QuitWhenIdle();
        m_quit_future.wait();
    }
}

int MessageLooper::RegisterTimer
    (
    int a_milliseconds,
    std::function<bool( void )> a_fun
    )
{
    std::unique_lock<std::shared_mutex> lcker( m_mutex );
    int index = m_control_block->_current_timer_index++;

    if( m_running_control_block && m_running_control_block->m_is_running )
    {
        std::function<void()> tsk;
        tsk = [ index, a_fun, a_milliseconds, this ]()
            {
                std::shared_ptr<base::RepeatingTimer> timer = std::make_shared<base::RepeatingTimer>();
                m_running_control_block->_timers[index] = timer;
                timer->Start( FROM_HERE, base::TimeDelta::FromMilliseconds( a_milliseconds ),
                              base::Bind( &MessageLooper::timer_function_wrapper, base::Unretained( this ), index, a_fun ) );
            };
    }
    else
    {
        PendingTimerBlock timer_block;
        timer_block.m_index = index;
        timer_block.m_callback = a_fun;
        timer_block.m_interval_milliseconds = a_milliseconds;
        m_control_block->m_timer_to_register.push_back( timer_block );
    }

    return index;
}

void MessageLooper::PostTask( std::function<void()> a_task )
{
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    if( m_running_control_block )
    {
        m_running_control_block->_msgLoop.task_runner()->PostTask(
            FROM_HERE, base::Bind( &___task_wrapper, a_task ) );
    }
    else
    {
        m_control_block->m_pending_task.push_back( a_task );
    }
}

void MessageLooper::PostDelayTask
    (
    int a_milliseconds,
    std::function<void()> a_task
    )
{
    base::TimeDelta delay = base::TimeDelta::FromMilliseconds( a_milliseconds );
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    if( m_running_control_block )
    {
        m_running_control_block->_msgLoop.task_runner()->PostDelayedTask(
            FROM_HERE, base::Bind( &___task_wrapper, a_task ), delay );
    }
    else
    {
        LOG( ERROR ) << "Should not post pending task if runner not running.";
    }
}

bool MessageLooper::IsLooperThread()
{
    base::PlatformThreadId running_thread = base::PlatformThread::CurrentId();
    std::shared_lock<std::shared_mutex> lcker( m_mutex );
    if( m_running_control_block )
    {
        return running_thread == m_running_control_block->_running_thread_id;
    }
    else
    {
        return false;
    }
}

void MessageLooper::timer_function_wrapper( int index, std::function<bool( void )> a_fun )
{
    bool ret = a_fun();
    if( ret )
    {
        auto it = m_running_control_block->_timers.find( index );
        if( it == m_running_control_block->_timers.end() )
        {
            return;
        }

        it->second->Stop();

        m_running_control_block->_timers.erase( it );
    }
}

void MessageLooper::PostExitCallTask( std::function<void()> a_task )
{
    std::unique_lock<std::shared_mutex> lcker( m_mutex );
    m_control_block->m_exit_task.push_back( a_task );
}

