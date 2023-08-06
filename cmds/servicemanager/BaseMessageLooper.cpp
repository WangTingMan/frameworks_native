#include "BaseMessageLooper.h"

void BaseMessageLooper::timer_function_wrapper( int index, std::function<bool(void)> a_fun)
{
    bool ret = a_fun();
    if (ret)
    {
        auto it = _timers.find(index);
        if (it == _timers.end())
        {
            return;
        }

        it->second->Stop();

        _timers.erase(it);
    }
}

BaseMessageLooper::BaseMessageLooper()
{

}

void BaseMessageLooper::Run()
{
    _loop.Run();
}

int BaseMessageLooper::RegisterTimer
    (
    int a_milliseconds,
    std::function<bool(void)> a_fun
    )
{
    int index = _current_timer_index++;
    std::shared_ptr<base::RepeatingTimer> timer = std::make_shared<base::RepeatingTimer>();
    _timers[index] = timer;

    timer->Start(FROM_HERE, base::TimeDelta::FromMilliseconds(a_milliseconds),
        base::Bind(&BaseMessageLooper::timer_function_wrapper, base::Unretained(this), index, a_fun) );

    return index;
}

static void ___task_wrapper(std::function<void()> a_task)
{
    a_task();
}

void BaseMessageLooper::PostTask(std::function<void()> a_task)
{
    _msgLoop.task_runner()->PostTask(FROM_HERE, base::Bind(&___task_wrapper, a_task));
}