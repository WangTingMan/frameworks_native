#pragma once
#include <base\system_monitor\system_monitor.h>
#include <base\message_loop\message_loop.h>
#include <base\observer_list_threadsafe.h>
#include <base\memory\scoped_refptr.h>
#include <base\time\time.h>
#include <base\timer\timer.h>

#include <functional>
#include <map>
#include <memory>

class BaseMessageLooper
{

public:

    BaseMessageLooper();

    void Run();

    int RegisterTimer
        (
        int a_milliseconds,
        std::function<bool(void)> a_fun
        );

    void PostTask(std::function<void()> a_task);

private:

    void timer_function_wrapper(int index, std::function<bool(void)> a_fun);

    base::MessageLoop _msgLoop;
    base::RunLoop _loop;
    std::map<int, std::shared_ptr<base::RepeatingTimer>> _timers;
    int _current_timer_index = 0;
};

