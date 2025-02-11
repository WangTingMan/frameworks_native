/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/Status.h>
#include <utils/Looper.h>
#include <utils/StrongPointer.h>

#include "Access.h"
#include "ServiceManager.h"

<<<<<<< HEAD
#ifdef _MSC_VER
#include <windows.h>
#include "linux/MessageLooper.h"
#include <base/process/process.h>
#include <base/process/process_handle.h>
#include <base/process/launch.h>
#include <base/logging.h>
#endif

#include <functional>
#include <memory>

#include <linux/binder.h>

#ifdef ERROR
#undef ERROR
#endif

#ifdef _MSC_VER
extern void load_hw_service_manager();
#endif
=======
#if !defined(VENDORSERVICEMANAGER) && !defined(__ANDROID_RECOVERY__)

#include <perfetto/public/producer.h>
#include <perfetto/public/te_category_macros.h>
#include <perfetto/public/te_macros.h>
#include <perfetto/public/track_event.h>

namespace android {

static void register_perfetto_te_categories() {
    struct PerfettoProducerInitArgs perfetto_args = PERFETTO_PRODUCER_INIT_ARGS_INIT();
    perfetto_args.backends = PERFETTO_BACKEND_SYSTEM;
    PerfettoProducerInit(perfetto_args);
    PerfettoTeInit();
    PERFETTO_TE_REGISTER_CATEGORIES(PERFETTO_SM_CATEGORIES);
}
} // namespace android

#endif // !defined(VENDORSERVICEMANAGER) && !defined(__ANDROID_RECOVERY__)
>>>>>>> d3fb93fb73

using ::android::Access;
using ::android::IPCThreadState;
using ::android::Looper;
using ::android::LooperCallback;
using ::android::ProcessState;
using ::android::ServiceManager;
using ::android::sp;
using ::android::base::SetProperty;
using ::android::os::IServiceManager;

bool libchrome_logging_handler( int levelIn, const char* file, int line,
                         size_t message_start, const std::string& str );

class BinderCallback : public LooperCallback {
public:
    static sp<BinderCallback> setupTo(const sp<Looper>& looper) {
        sp<BinderCallback> cb = sp<BinderCallback>::make();
        cb->mLooper = looper;

        IPCThreadState::self()->setupPolling(&cb->mBinderFd);
        LOG_ALWAYS_FATAL_IF(cb->mBinderFd < 0, "Failed to setupPolling: %d", cb->mBinderFd);

        int ret = looper->addFd(cb->mBinderFd, Looper::POLL_CALLBACK, Looper::EVENT_INPUT, cb,
                                nullptr /*data*/);
        LOG_ALWAYS_FATAL_IF(ret != 1, "Failed to add binder FD to Looper");

        return cb;
    }

    int handleEvent(int /* fd */, int /* events */, void* /* data */) override {
        IPCThreadState::self()->handlePolledCommands();
        return 1;  // Continue receiving callbacks.
    }

    void repoll() {
        if (!mLooper->repoll(mBinderFd)) {
            ALOGE("Failed to repoll binder FD.");
        }
    }

private:
    sp<Looper> mLooper;
    int mBinderFd = -1;
};

// LooperCallback for IClientCallback
#ifndef _MSC_VER
class ClientCallbackCallback : public LooperCallback {
public:
    static sp<ClientCallbackCallback> setupTo(const sp<Looper>& looper,
                                              const sp<ServiceManager>& manager,
                                              sp<BinderCallback> binderCallback) {
        sp<ClientCallbackCallback> cb = sp<ClientCallbackCallback>::make(manager);
        cb->mBinderCallback = binderCallback;

        int fdTimer = 0;
        fdTimer = timerfd_create(CLOCK_MONOTONIC, 0 /*flags*/);
        LOG_ALWAYS_FATAL_IF(fdTimer < 0, "Failed to timerfd_create: fd: %d err: %d", fdTimer, errno);

        itimerspec timespec {
            .it_interval = {
                .tv_sec = 5,
                .tv_nsec = 0,
            },
            .it_value = {
                .tv_sec = 5,
                .tv_nsec = 0,
            },
        };

        int timeRes = 0;

        timeRes = timerfd_settime(fdTimer, 0 /*flags*/, &timespec, nullptr);
        LOG_ALWAYS_FATAL_IF(timeRes < 0, "Failed to timerfd_settime: res: %d err: %d", timeRes, errno);

        int addRes = looper->addFd(fdTimer,
                                   Looper::POLL_CALLBACK,
                                   Looper::EVENT_INPUT,
                                   cb,
                                   nullptr);
        LOG_ALWAYS_FATAL_IF(addRes != 1, "Failed to add client callback FD to Looper");

        return cb;
    }

    int handleEvent(int fd, int /*events*/, void* /*data*/) override {

        uint64_t expirations;
        int ret = 0;
        read(fd, &expirations, sizeof(expirations));

        if (ret != sizeof(expirations)) {
            ALOGE("Read failed to callback FD: ret: %d err: %d", ret, errno);
        }

        mManager->handleClientCallbacks();
        mBinderCallback->repoll(); // b/316829336

        return 1;  // Continue receiving callbacks.
    }
private:
    friend sp<ClientCallbackCallback>;
    ClientCallbackCallback(const sp<ServiceManager>& manager) : mManager(manager) {}
    sp<ServiceManager> mManager;
    sp<BinderCallback> mBinderCallback;
};
#else
class ClientCallbackCallback : public virtual android::RefBase
{
public:

    inline static constexpr int s_handle_interval_ms = 5000;

    ClientCallbackCallback(const sp<ServiceManager>& manager)
    {
        mManager = manager;
    }

    bool handleEvent()
    {
        mManager->handleClientCallbacks();
        return false;
    }

private:

    sp<ServiceManager> mManager;
};
#endif

#ifdef _MSC_VER
BOOL WINAPI HandlerRoutineReceiver(DWORD dwCtrlType);
#endif

int main(int argc, char** argv) {
    android::base::InitLogging(argv, android::base::KernelLogger);

#ifdef _MSC_VER
    SetConsoleCtrlHandler(&HandlerRoutineReceiver, TRUE);
#endif

    logging::SetLogMessageHandler( libchrome_logging_handler );
    __set_default_log_file_name( nullptr, false );

    if (argc > 2) {
        LOG(FATAL) << "usage: " << argv[0] << " [binder driver]";
    }

    const char* driver = argc == 2 ? argv[1] : "/dev/binder";

#if !defined(VENDORSERVICEMANAGER) && !defined(__ANDROID_RECOVERY__)
    android::register_perfetto_te_categories();
#endif // !defined(VENDORSERVICEMANAGER) && !defined(__ANDROID_RECOVERY__)

    LOG(INFO) << "Starting sm instance on " << driver;

    sp<ProcessState> ps = ProcessState::initWithDriver(driver);
    ps->setThreadPoolMaxThreadCount(0);
    ps->setCallRestriction(ProcessState::CallRestriction::FATAL_IF_NOT_ONEWAY);

    IPCThreadState::self()->disableBackgroundScheduling(true);

    sp<ServiceManager> manager = sp<ServiceManager>::make(std::make_unique<Access>());
    if (!manager->addService("manager", manager, false /*allowIsolated*/, IServiceManager::DUMP_FLAG_PRIORITY_DEFAULT).isOk()) {
        LOG(ERROR) << "Could not self register servicemanager";
    }

    IPCThreadState::self()->setTheContextObject(manager);
    if (!ps->becomeContextManager()) {
        LOG(FATAL) << "Could not become context manager";
    }

#ifdef _MSC_VER
    MessageLooper& looper = MessageLooper::GetDefault();
    std::function<bool()> timer_callback = std::bind(&ClientCallbackCallback::handleEvent,
        std::make_shared<ClientCallbackCallback>(manager));
    looper.RegisterTimer(ClientCallbackCallback::s_handle_interval_ms, timer_callback);
    auto fun = [&looper]()
        {
            looper.PostTask( []()
            {
                IPCThreadState::self()->handlePolledCommands();
            } );
        };

    porting_binder::register_binder_data_handler( fun );

    int binder_fd = -1;
    IPCThreadState::self()->setupPolling(&binder_fd);
    LOG_ALWAYS_FATAL_IF(binder_fd < 0, "Failed to setupPolling: %d", binder_fd);

    IPCThreadState::self()->registerAsyncFdEventHandler( fun );
    fun();

#else
    sp<Looper> looper = Looper::prepare(false /*allowNonCallbacks*/);

<<<<<<< HEAD
    BinderCallback::setupTo(looper);
    ClientCallbackCallback::setupTo(looper, manager);
#endif
=======
    sp<BinderCallback> binderCallback = BinderCallback::setupTo(looper);
    ClientCallbackCallback::setupTo(looper, manager, binderCallback);
>>>>>>> d3fb93fb73

#ifndef VENDORSERVICEMANAGER
    if (!SetProperty("servicemanager.ready", "true")) {
        LOG(ERROR) << "Failed to set servicemanager ready property";
    }
#endif

#ifdef _MSC_VER
    load_hw_service_manager();
    looper.Run();
    base::LaunchOptions option;
    option.wait = false;
    std::wstring cmd{ L"taskkill /pid " };
    cmd.append(std::to_wstring(base::GetCurrentProcId()));
    cmd.append(L" /f");
    base::LaunchProcess(cmd, option);
#else
    while(true) {
        looper->pollAll(-1);
    }
#endif
    // should not be reached
    return EXIT_FAILURE;
}

bool libchrome_logging_handler( int levelIn, const char* file, int line,
                         size_t message_start, const std::string& str )
{
    android_LogPriority level = android_LogPriority::ANDROID_LOG_DEFAULT;

    switch( levelIn )
    {

    case logging::LOG_VERBOSE:
        level = android_LogPriority::ANDROID_LOG_VERBOSE;
    break;
    case logging::LOG_INFO:
        level = android_LogPriority::ANDROID_LOG_INFO;
    break;
    case logging::LOG_WARNING:
        level = android_LogPriority::ANDROID_LOG_WARN;
    break;
    case logging::LOG_ERROR:
        level = android_LogPriority::ANDROID_LOG_ERROR;
    break;
    case logging::LOG_FATAL:
        level = android_LogPriority::ANDROID_LOG_FATAL;
    break;
    case logging::LOG_NUM_SEVERITIES:
        level = android_LogPriority::ANDROID_LOG_VERBOSE;
    break;
        default:
    break;
    }

    std::string logStr;
    if( str.size() > message_start )
    {
        logStr = str.substr( message_start );
    }
#ifdef __android_log_print_ext_defined
    __android_log_print_ext( level, nullptr, file, line, logStr.c_str() );
#else
    __android_log_print(level, nullptr, logStr.c_str());
#endif

    return true;
}

#ifdef _MSC_VER
BOOL WINAPI HandlerRoutineReceiver(DWORD dwCtrlType)
{
    MessageLooper::GetDefault().Quit();
    return FALSE;
}
#endif
