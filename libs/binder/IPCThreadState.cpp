/*
 * Copyright (C) 2005 The Android Open Source Project
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

#define LOG_TAG "IPCThreadState"

#include <binder/IPCThreadState.h>

#include <binder/Binder.h>
#include <binder/BpBinder.h>
#include <binder/TextOutput.h>

#include <android-base/macros.h>
#include <cutils/sched_policy.h>
#include <utils/CallStack.h>
#include <utils/Log.h>
#include <utils/SystemClock.h>

#include <atomic>
#include <errno.h>
#include <inttypes.h>
#include <thread>
#include <signal.h>
#include <stdio.h> 
#ifndef _MSC_VER
#include <sched.h>
#include <sys/ioctl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <linux/MessageLooper.h>
#else
#include <binder/windows_porting.h>
#include <linux/binder.h>
#include <memory>
#include <mutex>
#include <linux/MessageLooper.h>
#include "servicemanager/ServiceManager.h"
#include "binder_driver/ipc_connection_token.h"
#endif

#include "Static.h"
#include "binder_module.h"

#include <cutils\threads.h>

#ifdef _MSC_VER
#define LOG_NDEBUG 1
#endif

#if LOG_NDEBUG

#define IF_LOG_TRANSACTIONS() if (false)
#define IF_LOG_COMMANDS() if (false)
#define LOG_REMOTEREFS(...) 
#define IF_LOG_REMOTEREFS() if (false)

#define LOG_THREADPOOL(...) 
#define LOG_ONEWAY(...) 

#else

#define IF_LOG_TRANSACTIONS() IF_ALOG(LOG_VERBOSE, "transact")
#define IF_LOG_COMMANDS() IF_ALOG(LOG_VERBOSE, "ipc")
#define LOG_REMOTEREFS(...) ALOG(LOG_DEBUG, "remoterefs", __VA_ARGS__)
#define IF_LOG_REMOTEREFS() IF_ALOG(LOG_DEBUG, "remoterefs")
#define LOG_THREADPOOL(...) ALOG(LOG_DEBUG, "threadpool", __VA_ARGS__)
#define LOG_ONEWAY(...) ALOG(LOG_DEBUG, "ipc", __VA_ARGS__)

#endif

#define _IOC_NRBITS 8
#define _IOC_NRMASK ((1 << _IOC_NRBITS) - 1)

// ---------------------------------------------------------------------------

namespace android {

// Static const and functions will be optimized out if not used,
// when LOG_NDEBUG and references in IF_LOG_COMMANDS() are optimized out.
static const char *kReturnStrings[] = {
    "BR_ERROR",
    "BR_OK",
    "BR_TRANSACTION",
    "BR_REPLY",
    "BR_ACQUIRE_RESULT",
    "BR_DEAD_REPLY",
    "BR_TRANSACTION_COMPLETE",
    "BR_INCREFS",
    "BR_ACQUIRE",
    "BR_RELEASE",
    "BR_DECREFS",
    "BR_ATTEMPT_ACQUIRE",
    "BR_NOOP",
    "BR_SPAWN_LOOPER",
    "BR_FINISHED",
    "BR_DEAD_BINDER",
    "BR_CLEAR_DEATH_NOTIFICATION_DONE",
    "BR_FAILED_REPLY",
    "BR_FROZEN_REPLY",
    "BR_ONEWAY_SPAM_SUSPECT",
    "BR_TRANSACTION_SEC_CTX",
};

static const char *kCommandStrings[] = {
    "BC_TRANSACTION",
    "BC_REPLY",
    "BC_ACQUIRE_RESULT",
    "BC_FREE_BUFFER",
    "BC_INCREFS",
    "BC_ACQUIRE",
    "BC_RELEASE",
    "BC_DECREFS",
    "BC_INCREFS_DONE",
    "BC_ACQUIRE_DONE",
    "BC_ATTEMPT_ACQUIRE",
    "BC_REGISTER_LOOPER",
    "BC_ENTER_LOOPER",
    "BC_EXIT_LOOPER",
    "BC_REQUEST_DEATH_NOTIFICATION",
    "BC_CLEAR_DEATH_NOTIFICATION",
    "BC_DEAD_BINDER_DONE"
};

static const int64_t kWorkSourcePropagatedBitIndex = 32;

static const char* getReturnString(uint32_t cmd)
{
    size_t idx = cmd & _IOC_NRMASK;
    if (idx < sizeof(kReturnStrings) / sizeof(kReturnStrings[0]))
        return kReturnStrings[idx];
    else
        return "unknown";
}

static const void* printBinderTransactionData(TextOutput& out, const void* data)
{
    const binder_transaction_data* btd =
        (const binder_transaction_data*)data;
    if (btd->target.binder_handle < 1024) {
        /* want to print descriptors in decimal; guess based on value */
        out << "target.desc=" << btd->target.binder_handle;
    } else {
        out << "target.ptr=" << btd->target.binder_target_ptr;
    }
    out << " (cookie " << btd->binder_transaction_cookie << ")" << endl
        << "code=" << TypeCode(btd->code) << ", flags=" << (void*)(uint64_t)btd->flags << endl
        << "data=" << btd->data.ptr.buffer << " (" << (void*)btd->data_size
        << " bytes)" << endl
        << "offsets=" << btd->data.ptr.offsets << " (" << (void*)btd->offsets_size
        << " bytes)";
    return btd+1;

}

static const void* printReturnCommand(TextOutput& out, const void* _cmd)
{
    static const size_t N = sizeof(kReturnStrings)/sizeof(kReturnStrings[0]);
    const int32_t* cmd = (const int32_t*)_cmd;
    uint32_t code = (uint32_t)*cmd++;
    size_t cmdIndex = code & 0xff;
    if (code == BR_ERROR) {
        out << "BR_ERROR: " << (void*)(uint64_t)(*cmd++) << endl;
        return cmd;
    } else if (cmdIndex >= N) {
        out << "Unknown reply: " << code << endl;
        return cmd;
    }
    out << kReturnStrings[cmdIndex];

    switch (code) {
        case BR_TRANSACTION:
        case BR_REPLY: {
            out << ": " << indent;
            cmd = (const int32_t *)printBinderTransactionData(out, cmd);
            out << dedent;
        } break;

        case BR_ACQUIRE_RESULT: {
            const int32_t res = *cmd++;
            out << ": " << res << (res ? " (SUCCESS)" : " (FAILURE)");
        } break;

        case BR_INCREFS:
        case BR_ACQUIRE:
        case BR_RELEASE:
        case BR_DECREFS: {
            const int32_t b = *cmd++;
            const int32_t c = *cmd++;
            out << ": target=" << (void*)(uint64_t)b << " (cookie " << (void*)(uint64_t)c << ")";
        } break;

        case BR_ATTEMPT_ACQUIRE: {
            const int32_t p = *cmd++;
            const int32_t b = *cmd++;
            const int32_t c = *cmd++;
            out << ": target=" << (void*)(uint64_t)b << " (cookie " << (void*)(uint64_t)c
                << "), pri=" << p;
        } break;

        case BR_DEAD_BINDER:
        case BR_CLEAR_DEATH_NOTIFICATION_DONE: {
            const int32_t c = *cmd++;
            out << ": death cookie " << (void*)(uint64_t)c;
        } break;

        default:
            // no details to show for: BR_OK, BR_DEAD_REPLY,
            // BR_TRANSACTION_COMPLETE, BR_FINISHED
            break;
    }
    out << endl;
    return cmd;
}

static const void* printCommand(TextOutput& out, const void* _cmd)
{
    static const size_t N = sizeof(kCommandStrings)/sizeof(kCommandStrings[0]);
    const int32_t* cmd = (const int32_t*)_cmd;
    uint32_t code = (uint32_t)*cmd++;
    size_t cmdIndex = code & 0xff;

    if (cmdIndex >= N) {
        out << "Unknown command: " << code << endl;
        return cmd;
    }
    out << kCommandStrings[cmdIndex];
    switch (code) {
        case BC_TRANSACTION:
        case BC_REPLY: {
            out << ": " << indent;
            cmd = (const int32_t *)printBinderTransactionData(out, cmd);
            out << dedent;
        } break;

        case BC_ACQUIRE_RESULT: {
            const int32_t res = *cmd++;
            out << ": " << res << (res ? " (SUCCESS)" : " (FAILURE)");
        } break;

        case BC_FREE_BUFFER: {
            const int32_t buf = *cmd++;
            out << ": buffer=" << (void*)(uint64_t)buf;
        } break;

        case BC_INCREFS:
        case BC_ACQUIRE:
        case BC_RELEASE:
        case BC_DECREFS: {
            const int32_t d = *cmd++;
            out << ": desc=" << d;
        } break;

        case BC_INCREFS_DONE:
        case BC_ACQUIRE_DONE: {
            const int32_t b = *cmd++;
            const int32_t c = *cmd++;
            out << ": target=" << (void*)(uint64_t)b << " (cookie " << (void*)(uint64_t)c << ")";
        } break;

        case BC_ATTEMPT_ACQUIRE: {
            const int32_t p = *cmd++;
            const int32_t d = *cmd++;
            out << ": desc=" << d << ", pri=" << p;
        } break;

        case BC_REQUEST_DEATH_NOTIFICATION:
        case BC_CLEAR_DEATH_NOTIFICATION: {
            const int32_t h = *cmd++;
            const int32_t c = *cmd++;
            out << ": handle=" << h << " (death cookie " << (void*)(uint64_t)c << ")";
        } break;

        case BC_DEAD_BINDER_DONE: {
            const int32_t c = *cmd++;
            out << ": death cookie " << (void*)(uint64_t)c;
        } break;

        default:
            // no details to show for: BC_REGISTER_LOOPER, BC_ENTER_LOOPER,
            // BC_EXIT_LOOPER
            break;
    }
    out << endl;
    return cmd;
}

static std::mutex gTLSMutex;
static std::atomic<bool> gHaveTLS(false);
static std::atomic<bool> gShutdown = false;
static std::atomic<bool> gDisableBackgroundScheduling = false;

#ifndef _MSC_VER
static pthread_key_t gTLS = 0;
#else
thread_local static bool gValid = false;
thread_local static std::shared_ptr<IPCThreadState> gTLS;
#endif

IPCThreadState* IPCThreadState::self()
{
    if (gHaveTLS.load(std::memory_order_acquire)) {
    restart:
#ifndef _MSC_VER
        const pthread_key_t k = gTLS;
        IPCThreadState* st = (IPCThreadState*)pthread_getspecific(k);
        if (st) return st;
#else
        if (gTLS) {
            if (!gValid) {
                return nullptr;
            }
            return gTLS.get();
        }
#endif
        return new IPCThreadState;
    }

    // Racey, heuristic test for simultaneous shutdown.
    if (gShutdown.load(std::memory_order_relaxed)) {
        ALOGW("Calling IPCThreadState::self() during shutdown is dangerous, expect a crash.\n");
        return nullptr;
    }

    std::lock_guard<std::mutex> pthread_mutex_locker(gTLSMutex);
    if (!gHaveTLS.load(std::memory_order_relaxed)) {
#ifndef _MSC_VER
        int key_create_value = pthread_key_create(&gTLS, threadDestructor);
        if (key_create_value != 0) {
            ALOGW("IPCThreadState::self() unable to create TLS key, expect a crash: %s\n",
                    strerror(key_create_value));
            return nullptr;
        }
#endif
        gHaveTLS.store(true, std::memory_order_release);
    }

    goto restart;
}

#ifdef _MSC_VER
void IPCThreadState::postTask( std::function<void()> a_tsk )
{
    MessageLooper::GetDefault().PostTask( a_tsk );
}

void IPCThreadState::postDelayTask( uint32_t a_delayed_ms, std::function<void()> a_tsk )
{
    MessageLooper::GetDefault().PostDelayTask( a_delayed_ms, a_tsk );
}
#endif

IPCThreadState* IPCThreadState::selfOrNull()
{
    if (gHaveTLS.load(std::memory_order_acquire)) {
#ifndef _MSC_VER
        const pthread_key_t k = gTLS;
        IPCThreadState* st = (IPCThreadState*)pthread_getspecific(k);
        return st;
#else
        return gTLS.get();
#endif
    }
    return nullptr;
}

void IPCThreadState::shutdown()
{
    gShutdown.store(true, std::memory_order_relaxed);

    if (gHaveTLS.load(std::memory_order_acquire)) {
        // XXX Need to wait for all thread pool threads to exit!
#ifndef _MSC_VER
        IPCThreadState* st = (IPCThreadState*)pthread_getspecific(gTLS);
        if (st) {
            delete st;
            pthread_setspecific(gTLS, nullptr);
        }
        pthread_key_delete(gTLS);
#else
        gTLS.reset();
#endif
        gHaveTLS.store(false, std::memory_order_release);
    }
}

void IPCThreadState::disableBackgroundScheduling(bool disable)
{
    gDisableBackgroundScheduling.store(disable, std::memory_order_relaxed);
}

bool IPCThreadState::backgroundSchedulingDisabled()
{
    return gDisableBackgroundScheduling.load(std::memory_order_relaxed);
}

status_t IPCThreadState::clearLastError()
{
    const status_t err = mLastError;
    mLastError = NO_ERROR;
    return err;
}

pid_t IPCThreadState::getCallingPid() const
{
    checkContextIsBinderForUse(__func__);
    return mCallingPid;
}

const char* IPCThreadState::getCallingSid() const
{
    checkContextIsBinderForUse(__func__);
    return mCallingSid;
}

uid_t IPCThreadState::getCallingUid() const
{
    checkContextIsBinderForUse(__func__);
    return mCallingUid;
}

const IPCThreadState::SpGuard* IPCThreadState::pushGetCallingSpGuard(const SpGuard* guard) {
    const SpGuard* orig = mServingStackPointerGuard;
    mServingStackPointerGuard = guard;
    return orig;
}

void IPCThreadState::restoreGetCallingSpGuard(const SpGuard* guard) {
    mServingStackPointerGuard = guard;
}

void IPCThreadState::checkContextIsBinderForUse(const char* use) const {
    if (LIKELY(mServingStackPointerGuard == nullptr)) return;

    if (!mServingStackPointer || mServingStackPointerGuard->address < mServingStackPointer) {
        LOG_ALWAYS_FATAL("In context %s, %s does not make sense (binder sp: %p, guard: %p).",
                         mServingStackPointerGuard->context, use, mServingStackPointer,
                         mServingStackPointerGuard->address);
    }

    // in the case mServingStackPointer is deeper in the stack than the guard,
    // we must be serving a binder transaction (maybe nested). This is a binder
    // context, so we don't abort
}

int64_t IPCThreadState::clearCallingIdentity()
{
    // ignore mCallingSid for legacy reasons
    int64_t token = ((int64_t)mCallingUid<<32) | mCallingPid;
    clearCaller();
    return token;
}

void IPCThreadState::setStrictModePolicy(int32_t policy)
{
    mStrictModePolicy = policy;
}

int32_t IPCThreadState::getStrictModePolicy() const
{
    return mStrictModePolicy;
}

int64_t IPCThreadState::setCallingWorkSourceUid(uid_t uid)
{
    int64_t token = setCallingWorkSourceUidWithoutPropagation(uid);
    mPropagateWorkSource = true;
    return token;
}

int64_t IPCThreadState::setCallingWorkSourceUidWithoutPropagation(uid_t uid)
{
    const int64_t propagatedBit = ((int64_t)mPropagateWorkSource) << kWorkSourcePropagatedBitIndex;
    int64_t token = propagatedBit | mWorkSource;
    mWorkSource = uid;
    return token;
}

void IPCThreadState::clearPropagateWorkSource()
{
    mPropagateWorkSource = false;
}

bool IPCThreadState::shouldPropagateWorkSource() const
{
    return mPropagateWorkSource;
}

uid_t IPCThreadState::getCallingWorkSourceUid() const
{
    return mWorkSource;
}

int64_t IPCThreadState::clearCallingWorkSource()
{
    return setCallingWorkSourceUid(kUnsetWorkSource);
}

void IPCThreadState::restoreCallingWorkSource(int64_t token)
{
    uid_t uid = (int)token;
    setCallingWorkSourceUidWithoutPropagation(uid);
    mPropagateWorkSource = ((token >> kWorkSourcePropagatedBitIndex) & 1) == 1;
}

void IPCThreadState::setLastTransactionBinderFlags(int32_t flags)
{
    mLastTransactionBinderFlags = flags;
}

int32_t IPCThreadState::getLastTransactionBinderFlags() const
{
    return mLastTransactionBinderFlags;
}

void IPCThreadState::setCallRestriction(ProcessState::CallRestriction restriction) {
    mCallRestriction = restriction;
}

ProcessState::CallRestriction IPCThreadState::getCallRestriction() const {
    return mCallRestriction;
}

void IPCThreadState::restoreCallingIdentity(int64_t token)
{
    mCallingUid = (int)(token>>32);
    mCallingSid = nullptr;  // not enough data to restore
    mCallingPid = (int)token;
}

void IPCThreadState::clearCaller()
{
    mCallingPid = getpid();
    mCallingSid = nullptr;  // expensive to lookup
    mCallingUid = getuid();
}

void IPCThreadState::flushCommands()
{
    if (mProcess->mDriverFD < 0)
        return;
    talkWithDriver(false);
    // The flush could have caused post-write refcount decrements to have
    // been executed, which in turn could result in BC_RELEASE/BC_DECREFS
    // being queued in mOut. So flush again, if we need to.
    if (mOut.dataSize() > 0) {
        talkWithDriver(false);
    }
    if (mOut.dataSize() > 0) {
        ALOGW("mOut.dataSize() > 0 after flushCommands()");
    }
}

bool IPCThreadState::flushIfNeeded()
{
    if (mIsLooper || mServingStackPointer != nullptr || mIsFlushing) {
        return false;
    }
    mIsFlushing = true;
    // In case this thread is not a looper and is not currently serving a binder transaction,
    // there's no guarantee that this thread will call back into the kernel driver any time
    // soon. Therefore, flush pending commands such as BC_FREE_BUFFER, to prevent them from getting
    // stuck in this thread's out buffer.
    flushCommands();
    mIsFlushing = false;
    return true;
}

void IPCThreadState::blockUntilThreadAvailable()
{
    std::unique_lock<std::mutex> thread_mutex_lock(mProcess->mThreadCountLock);
    mProcess->mWaitingForThreads++;
    while (mProcess->mExecutingThreadsCount >= mProcess->mMaxThreads) {
        ALOGW("Waiting for thread to be free. mExecutingThreadsCount=%lu mMaxThreads=%lu\n",
                static_cast<unsigned long>(mProcess->mExecutingThreadsCount),
                static_cast<unsigned long>(mProcess->mMaxThreads));
        mProcess->mThreadCountDecrement.wait(thread_mutex_lock);
    }
    mProcess->mWaitingForThreads--;
}

status_t IPCThreadState::getAndExecuteCommand()
{
    status_t result;
    int32_t cmd;

    result = talkWithDriver();

    if (result >= NO_ERROR) {
        size_t IN_ = mIn.dataAvail();
        if (IN_ < sizeof(int32_t)) return result;
        cmd = mIn.readInt32();
        IF_LOG_COMMANDS() {
            alog.setSourceLocation(__FILE__, __LINE__);
            alog << "Processing top-level Command: "
                 << getReturnString(cmd) << endl;
        }

        std::unique_lock<std::mutex> locker(mProcess->mThreadCountLock);
        mProcess->mExecutingThreadsCount++;
        if (mProcess->mExecutingThreadsCount >= mProcess->mMaxThreads &&
                mProcess->mStarvationStartTimeMs == 0) {
            mProcess->mStarvationStartTimeMs = uptimeMillis();
        }
        locker.unlock();

        result = executeCommand(cmd);

        locker.lock();
        mProcess->mExecutingThreadsCount--;
        if (mProcess->mExecutingThreadsCount < mProcess->mMaxThreads &&
                mProcess->mStarvationStartTimeMs != 0) {
            int64_t starvationTimeMs = uptimeMillis() - mProcess->mStarvationStartTimeMs;
            if (starvationTimeMs > 100) {
                ALOGE("binder thread pool (%zu threads) starved for %" PRId64 " ms",
                      mProcess->mMaxThreads, starvationTimeMs);
            }
            mProcess->mStarvationStartTimeMs = 0;
        }

        // Cond broadcast can be expensive, so don't send it every time a binder
        // call is processed. b/168806193
        if (mProcess->mWaitingForThreads > 0) {
            mProcess->mThreadCountDecrement.notify_all();
        }
    }

    return result;
}

// When we've cleared the incoming command queue, process any pending derefs
void IPCThreadState::processPendingDerefs()
{
    if (mIn.dataPosition() >= mIn.dataSize()) {
        /*
         * The decWeak()/decStrong() calls may cause a destructor to run,
         * which in turn could have initiated an outgoing transaction,
         * which in turn could cause us to add to the pending refs
         * vectors; so instead of simply iterating, loop until they're empty.
         *
         * We do this in an outer loop, because calling decStrong()
         * may result in something being added to mPendingWeakDerefs,
         * which could be delayed until the next incoming command
         * from the driver if we don't process it now.
         */
        while (mPendingWeakDerefs.size() > 0 || mPendingStrongDerefs.size() > 0) {
            while (mPendingWeakDerefs.size() > 0) {
                RefBase::weakref_type* refs = mPendingWeakDerefs[0];
                mPendingWeakDerefs.removeAt(0);
                refs->decWeak(mProcess.get());
            }

            if (mPendingStrongDerefs.size() > 0) {
                // We don't use while() here because we don't want to re-order
                // strong and weak decs at all; if this decStrong() causes both a
                // decWeak() and a decStrong() to be queued, we want to process
                // the decWeak() first.
                BBinder* obj = mPendingStrongDerefs[0];
                mPendingStrongDerefs.removeAt(0);
                obj->decStrong(mProcess.get());
            }
        }
    }
}

void IPCThreadState::processPostWriteDerefs()
{
    for (size_t i = 0; i < mPostWriteWeakDerefs.size(); i++) {
        RefBase::weakref_type* refs = mPostWriteWeakDerefs[i];
        refs->decWeak(mProcess.get());
    }
    mPostWriteWeakDerefs.clear();

    for (size_t i = 0; i < mPostWriteStrongDerefs.size(); i++) {
        RefBase* obj = mPostWriteStrongDerefs[i];
        obj->decStrong(mProcess.get());
    }
    mPostWriteStrongDerefs.clear();
}

#ifdef _MSC_VER
void IPCThreadState::startThreadPoolImpl( bool a_is_main )
{
    LOG_THREADPOOL( "**** THREAD %p (PID %d) IS JOINING THE THREAD POOL\n", (void*)gettid(), getpid() );
    std::unique_lock<std::mutex> lcker( mProcess->mThreadCountLock );
    mProcess->mCurrentThreads++;
    lcker.unlock();
    mOut.writeInt32( a_is_main ? BC_ENTER_LOOPER : BC_REGISTER_LOOPER );

    mIsLooper = true;

    std::function<void()> fun;
    fun = [ this, a_is_main ]()mutable
        {
            status_t result = 0;
            processPendingDerefs();
            // now get the next command to be processed, waiting if necessary
            result = getAndExecuteCommand();
            constexpr int32_t ref_used = ECONNREFUSED;
            if( result < NO_ERROR && result != TIMED_OUT && result != -ECONNREFUSED && result != -EBADF )
            {
                LOG_ALWAYS_FATAL( "getAndExecuteCommand(fd=%d) returned unexpected error %d, aborting",
                                  mProcess->mDriverFD, result );
            }

            // Let this thread exit the thread pool if it is no longer
            // needed and it is not the main process thread.
            if( result == TIMED_OUT && !a_is_main )
            {
                ALOGE( "time out!" );
            }
        };

    std::function<void()> handler;
    handler = [ fun ]()
        {
            MessageLooper::GetDefault().PostTask( fun );
        };

    porting_binder::register_binder_data_handler( handler );
    MessageLooper::GetDefault().PostTask( fun );

    auto exit_fun = [ this ]()mutable
        {
            mOut.setDataPosition( 0 );
            mOut.setDataSize( 0 );
            mOut.writeInt32( BC_EXIT_LOOPER );
            mIsLooper = false;
            talkWithDriver( false );
            std::unique_lock<std::mutex> lcker( mProcess->mThreadCountLock );
            LOG_ALWAYS_FATAL_IF( mProcess->mCurrentThreads == 0,
                                 "Threadpool thread count = 0. Thread cannot exist and exit in empty "
                                 "threadpool\n"
                                 "Misconfiguration. Increase threadpool max threads configuration\n" );
            mProcess->mCurrentThreads--;
        };
    MessageLooper::GetDefault().PostExitCallTask( exit_fun );

    if( !MessageLooper::GetDefault().IsRunning() )
    {
        MessageLooper::GetDefault().Run();
    }
}
#endif

void IPCThreadState::joinThreadPool(bool isMain)
{
#ifdef _MSC_VER
    startThreadPoolImpl( isMain );
#else
    LOG_THREADPOOL("**** THREAD %p (PID %d) IS JOINING THE THREAD POOL\n", (void*)gettid(), getpid());
    std::unique_lock<std::mutex> lcker(mProcess->mThreadCountLock);
    mProcess->mCurrentThreads++;
    lcker.unlock();
    mOut.writeInt32(isMain ? BC_ENTER_LOOPER : BC_REGISTER_LOOPER);

    mIsLooper = true;
    status_t result = 0;
    do {
        processPendingDerefs();
        // now get the next command to be processed, waiting if necessary
        result = getAndExecuteCommand();

        constexpr int32_t ref_used = ECONNREFUSED;
        if (result < NO_ERROR && result != TIMED_OUT && result != -ECONNREFUSED && result != -EBADF) {
            LOG_ALWAYS_FATAL("getAndExecuteCommand(fd=%d) returned unexpected error %d, aborting",
                  mProcess->mDriverFD, result);
        }

        // Let this thread exit the thread pool if it is no longer
        // needed and it is not the main process thread.
        if(result == TIMED_OUT && !isMain) {
            break;
        }
    } while (result != -ECONNREFUSED && result != -EBADF);
    LOG_THREADPOOL("**** THREAD %p (PID %d) IS LEAVING THE THREAD POOL err=%d\n",
        (void*)gettid(), getpid(), result);

    mOut.writeInt32(BC_EXIT_LOOPER);
    mIsLooper = false;
    talkWithDriver(false);
    lcker.lock();
    LOG_ALWAYS_FATAL_IF(mProcess->mCurrentThreads == 0,
                        "Threadpool thread count = 0. Thread cannot exist and exit in empty "
                        "threadpool\n"
                        "Misconfiguration. Increase threadpool max threads configuration\n");
    mProcess->mCurrentThreads--;
#endif
}

status_t IPCThreadState::setupPolling(int* fd)
{
    if (mProcess->mDriverFD < 0) {
        return -EBADF;
    }

#ifdef _MSC_VER
    mOut.setDataPosition( 0 );
    mOut.setDataSize( 0 );
#endif
    mOut.writeInt32(BC_ENTER_LOOPER);
    flushCommands();
    *fd = mProcess->mDriverFD;
    std::unique_lock<std::mutex> lcker(mProcess->mThreadCountLock);
    mProcess->mCurrentThreads++;

    return 0;
}

status_t IPCThreadState::handlePolledCommands()
{
    status_t result;

    do {
        result = getAndExecuteCommand();
    } while (mIn.dataPosition() < mIn.dataSize());

    processPendingDerefs();
    flushCommands();
    return result;
}

void IPCThreadState::stopProcess(bool /*immediate*/)
{
    //ALOGI("**** STOPPING PROCESS");
    flushCommands();
    int fd = mProcess->mDriverFD;
    mProcess->mDriverFD = -1;
    close(fd);
    //kill(getpid(), SIGKILL);
}

status_t IPCThreadState::transact(int32_t handle,
                                  uint32_t code, const Parcel& data,
                                  Parcel* reply, uint32_t flags)
{
    LOG_ALWAYS_FATAL_IF(data.isForRpc(), "Parcel constructed for RPC, but being used with binder.");

    status_t err = 0;

    flags |= TF_ACCEPT_FDS;

    IF_LOG_TRANSACTIONS() {
        TextOutput::Bundle _b(alog);
        _b.setSourceLocation(__FILE__, __LINE__);
        alog << "BC_TRANSACTION thr " << gettid() << " / hand "
            << handle << " / code " << TypeCode(code) << ": "
            << indent << data << dedent << endl;
    }

    LOG_ONEWAY(">>>> SEND from pid %d uid %d %s", getpid(), getuid(),
        (flags & TF_ONE_WAY) == 0 ? "READ REPLY" : "ONE WAY");
    err = writeTransactionData(BC_TRANSACTION, flags, handle, code, data, nullptr);

    if (err != NO_ERROR) {
        if (reply) reply->setError(err);
        return (mLastError = err);
    }

    if ((flags & TF_ONE_WAY) == 0) {
        if (UNLIKELY(mCallRestriction != ProcessState::CallRestriction::NONE)) {
            if (mCallRestriction == ProcessState::CallRestriction::ERROR_IF_NOT_ONEWAY) {
                ALOGE("Process making non-oneway call (code: %u) but is restricted.", code);
                CallStack::logStack("non-oneway call", CallStack::getCurrent(10).get(),
                    ANDROID_LOG_ERROR);
            } else /* FATAL_IF_NOT_ONEWAY */ {
                LOG_ALWAYS_FATAL("Process may not make non-oneway calls (code: %u).", code);
            }
        }

        #if 0
        if (code == 4) { // relayout
            ALOGI(">>>>>> CALLING transaction 4");
        } else {
            ALOGI(">>>>>> CALLING transaction %d", code);
        }
        #endif
        if (reply) {
            err = waitForResponse(reply);
        } else {
            Parcel fakeReply;
            err = waitForResponse(&fakeReply);
        }
        #if 0
        if (code == 4) { // relayout
            ALOGI("<<<<<< RETURNING transaction 4");
        } else {
            ALOGI("<<<<<< RETURNING transaction %d", code);
        }
        #endif

        IF_LOG_TRANSACTIONS() {
            TextOutput::Bundle _b(alog);
            _b.setSourceLocation(__FILE__, __LINE__);
            alog << "BR_REPLY thr " << gettid() << " / hand "
                << handle << ": ";
            if (reply) alog << indent << *reply << dedent << endl;
            else alog << "(none requested)" << endl;
        }
    } else {
        ALOGI( "send binder message and wait for reply" );
        err = waitForResponse(nullptr, nullptr);
        ALOGI( "binder message sent and replied." );
    }
    return err;
}

void IPCThreadState::incStrongHandle(int32_t handle, BpBinder *proxy)
{

    LOG_REMOTEREFS( "IPCThreadState::incStrongHandle(%d)\n", handle );
#ifdef _MSC_VER
    mOut.setDataPosition( 0 );
    mOut.setDataSize( 0 );
#endif
    mOut.writeInt32(BC_ACQUIRE);
    mOut.writeInt32(handle);

    if (!flushIfNeeded()) {
        // Create a temp reference until the driver has handled this command.
        proxy->incStrong(mProcess.get());
        mPostWriteStrongDerefs.push(proxy);
    }
}

void IPCThreadState::decStrongHandle(int32_t handle)
{

    LOG_REMOTEREFS( "IPCThreadState::decStrongHandle(%d)\n", handle );
#ifdef _MSC_VER
    mOut.setDataPosition( 0 );
    mOut.setDataSize( 0 );
#endif
    mOut.writeInt32(BC_RELEASE);

    mOut.writeInt32(handle);
    flushIfNeeded();
}

void IPCThreadState::incWeakHandle(int32_t handle, BpBinder *proxy)
{

    LOG_REMOTEREFS( "IPCThreadState::incWeakHandle(%d)\n", handle );
#ifdef _MSC_VER
    mOut.setDataPosition( 0 );
    mOut.setDataSize( 0 );
#endif
    mOut.writeInt32(BC_INCREFS);

    mOut.writeInt32(handle);
    if (!flushIfNeeded()) {
        // Create a temp reference until the driver has handled this command.
        proxy->getWeakRefs()->incWeak(mProcess.get());
        mPostWriteWeakDerefs.push(proxy->getWeakRefs());
    }
}

void IPCThreadState::decWeakHandle(int32_t handle)
{

    LOG_REMOTEREFS("IPCThreadState::decWeakHandle(%d)\n", handle);
#ifdef _MSC_VER
    mOut.setDataPosition( 0 );
    mOut.setDataSize( 0 );
#endif
    mOut.writeInt32(BC_DECREFS);

    mOut.writeInt32(handle);
    flushIfNeeded();
}

status_t IPCThreadState::attemptIncStrongHandle(int32_t handle)
{
#if HAS_BC_ATTEMPT_ACQUIRE
    LOG_REMOTEREFS("IPCThreadState::attemptIncStrongHandle(%d)\n", handle);
    mOut.writeInt32(BC_ATTEMPT_ACQUIRE);
    mOut.writeInt32(0); // xxx was thread priority
    mOut.writeInt32(handle);
    status_t result = UNKNOWN_ERROR;

    waitForResponse(NULL, &result);

#if LOG_REFCOUNTS
    ALOGV("IPCThreadState::attemptIncStrongHandle(%ld) = %s\n",
        handle, result == NO_ERROR ? "SUCCESS" : "FAILURE");
#endif

    return result;
#else
    (void)handle;
    ALOGE("%s(%d): Not supported\n", __func__, handle);
    return INVALID_OPERATION;
#endif
}

void IPCThreadState::expungeHandle(int32_t handle, IBinder* binder)
{
#if LOG_REFCOUNTS
    ALOGV("IPCThreadState::expungeHandle(%ld)\n", handle);
#endif
    self()->mProcess->expungeHandle(handle, binder); // NOLINT
}

status_t IPCThreadState::requestDeathNotification(int32_t handle, BpBinder* proxy)
{
#ifdef _MSC_VER
    mOut.setDataPosition( 0 );
    mOut.setDataSize( 0 );
#endif
    mOut.writeInt32(BC_REQUEST_DEATH_NOTIFICATION);
    mOut.writeInt32((int32_t)handle);
    mOut.writePointer((uintptr_t)proxy);
#ifdef _MSC_VER
    String8 descriptor = String8(proxy->getInterfaceDescriptor());
    std::string std_descriptor( descriptor.string() );
    mOut.writeDynamic( std_descriptor );
#endif
    return NO_ERROR;
}

status_t IPCThreadState::clearDeathNotification(int32_t handle, BpBinder* proxy)
{
#ifdef _MSC_VER
    mOut.setDataPosition( 0 );
    mOut.setDataSize( 0 );
#endif
    mOut.writeInt32(BC_CLEAR_DEATH_NOTIFICATION);
    mOut.writeInt32((int32_t)handle);
    mOut.writePointer((uintptr_t)proxy);
#ifdef _MSC_VER
    String8 descriptor = String8( proxy->getInterfaceDescriptor() );
    std::string std_descriptor( descriptor.string() );
    mOut.writeDynamic( std_descriptor );
#endif
    return NO_ERROR;
}

IPCThreadState::IPCThreadState()
      : mProcess(ProcessState::self()),
        mServingStackPointer(nullptr),
        mServingStackPointerGuard(nullptr),
        mWorkSource(kUnsetWorkSource),
        mPropagateWorkSource(false),
        mIsLooper(false),
        mIsFlushing(false),
        mStrictModePolicy(0),
        mLastTransactionBinderFlags(0),
        mCallRestriction(mProcess->mCallRestriction) {
#ifndef _MSC_VER
    pthread_setspecific(gTLS, this);
#else
    gTLS = std::shared_ptr<IPCThreadState>(this, &threadDestructor);
    gValid = true;
#endif
    clearCaller();
#ifdef _MSC_VER
    mIn.setDataCapacity( 256 + INCREASED_TRANSACTION_DATA_SIZE );
    mOut.setDataCapacity( 256 + INCREASED_TRANSACTION_DATA_SIZE );
#else
    mIn.setDataCapacity(256);
    mOut.setDataCapacity(256);
#endif
}

IPCThreadState::~IPCThreadState()
{
#ifdef _MSC_VER
    gValid = false;
#endif
}

status_t IPCThreadState::sendReply(const Parcel& reply, uint32_t flags, binder_transaction_data* tr)
{
    status_t err;
    status_t statusBuffer;
#ifdef _MSC_VER
    err = writeTransactionData( BC_REPLY, flags, -1, 0, reply, &statusBuffer, tr );
#else
    err = writeTransactionData(BC_REPLY, flags, -1, 0, reply, &statusBuffer);
#endif
    if (err < NO_ERROR) return err;
    return waitForResponse(nullptr, nullptr);
}

status_t IPCThreadState::waitForResponse(Parcel *reply, status_t *acquireResult)
{
    uint32_t cmd;
    int32_t err;

    while (1) {
        if ((err=talkWithDriver()) < NO_ERROR) break;
        err = mIn.errorCheck();
        if (err < NO_ERROR) break;
        if (mIn.dataAvail() == 0) continue;

        cmd = (uint32_t)mIn.readInt32();
        IF_LOG_COMMANDS() {
            alog.setSourceLocation(__FILE__, __LINE__);
            alog << "Processing waitForResponse Command: "
                << getReturnString(cmd) << endl;
        }

        switch (cmd) {
        case BR_ONEWAY_SPAM_SUSPECT:
            ALOGE("Process seems to be sending too many oneway calls.");
            CallStack::logStack("oneway spamming", CallStack::getCurrent().get(),
                    ANDROID_LOG_ERROR);
            [[fallthrough]];
        case BR_TRANSACTION_COMPLETE:
            if (!reply && !acquireResult) goto finish;
            break;

        case BR_DEAD_REPLY:
            err = DEAD_OBJECT;
            goto finish;

        case BR_FAILED_REPLY:
            err = FAILED_TRANSACTION;
            goto finish;

        case BR_FROZEN_REPLY:
            err = FAILED_TRANSACTION;
            goto finish;

        case BR_ACQUIRE_RESULT:
            {
                ALOG_ASSERT(acquireResult != NULL, "Unexpected brACQUIRE_RESULT");
                const int32_t result = mIn.readInt32();
                if (!acquireResult) continue;
                *acquireResult = result ? NO_ERROR : INVALID_OPERATION;
            }
            goto finish;

        case BR_REPLY:
            {
                binder_transaction_data tr;
                err = mIn.read(&tr, sizeof(tr));
                ALOG_ASSERT(err == NO_ERROR, "Not enough command data for brREPLY");
                if (err != NO_ERROR) goto finish;

                if (reply) {
                    if ((tr.flags & TF_STATUS_CODE) == 0) {
                        reply->ipcSetDataReference(
                            reinterpret_cast<const uint8_t*>(tr.data.ptr.buffer),
                            tr.data_size,
                            reinterpret_cast<const binder_size_t*>(tr.data.ptr.offsets),
                            tr.offsets_size/sizeof(binder_size_t),
                            freeBuffer);
                    } else {
                        err = *reinterpret_cast<const status_t*>(tr.data.ptr.buffer);
                        freeBuffer(reinterpret_cast<const uint8_t*>(tr.data.ptr.buffer),
                                   tr.data_size,
                                   reinterpret_cast<const binder_size_t*>(tr.data.ptr.offsets),
                                   tr.offsets_size / sizeof(binder_size_t));
                    }
                } else {
                    freeBuffer(reinterpret_cast<const uint8_t*>(tr.data.ptr.buffer), tr.data_size,
                               reinterpret_cast<const binder_size_t*>(tr.data.ptr.offsets),
                               tr.offsets_size / sizeof(binder_size_t));
#ifdef _MSC_VER
                    ALOGI( "caller does not need reply. So we just return here." );
#else
                    continue;
#endif
                }
            }
            goto finish;

        default:
            err = executeCommand(cmd);
            if (err != NO_ERROR) goto finish;
            break;
        }
    }

finish:
    if (err != NO_ERROR) {
        if (acquireResult) *acquireResult = err;
        if (reply) reply->setError(err);
        mLastError = err;
        logExtendedError();
    }

    return err;
}

status_t IPCThreadState::talkWithDriver(bool doReceive)
{
    if (mProcess->mDriverFD < 0) {
        return -EBADF;
    }
    binder_write_read bwr;

#ifdef _MSC_VER
    mIn.setDataSize( 0 );
    mIn.setDataPosition( 0 );
#endif
    // Is the read buffer empty?
    const bool needRead = mIn.dataPosition() >= mIn.dataSize();

    // We don't want to write anything if we are still reading
    // from data left in the input buffer and the caller
    // has requested to read the next data.
    const size_t outAvail = (!doReceive || needRead) ? mOut.dataSize() : 0;

    bwr.write_size = outAvail;
    bwr.write_buffer = (uintptr_t)mOut.data();

    // This is what we'll read.
    if (doReceive && needRead) {
        bwr.read_size = mIn.dataCapacity();
        bwr.read_buffer = (uintptr_t)mIn.data();
    } else {
        bwr.read_size = 0;
        bwr.read_buffer = 0;
    }

    IF_LOG_COMMANDS() {
        TextOutput::Bundle _b(alog);
        _b.setSourceLocation(__FILE__, __LINE__);
        if (outAvail != 0) {
            alog << "Sending commands to driver: " << indent;
            const void* cmds = (const void*)bwr.write_buffer;
            const void* end = ((const uint8_t*)cmds)+bwr.write_size;
            alog << HexDump(cmds, bwr.write_size) << endl;
            while (cmds < end) cmds = printCommand(alog, cmds);
            alog << dedent;
        }
        alog << "Size of receive buffer: " << bwr.read_size
            << ", needRead: " << needRead << ", doReceive: " << doReceive << endl;
    }

    // Return immediately if there is nothing to do.
    if ((bwr.write_size == 0) && (bwr.read_size == 0)) return NO_ERROR;

    bwr.write_consumed = 0;
    bwr.read_consumed = 0;
    status_t err;
    do {
        IF_LOG_COMMANDS() {
            alog.setSourceLocation(__FILE__, __LINE__);
            alog << "About to read/write, write size = " << mOut.dataSize() << endl;
        }
#if defined(__ANDROID__)
        if (fcntl(mProcess->mDriverFD, BINDER_WRITE_READ, &bwr) >= 0)
            err = NO_ERROR;
        else
            err = -errno;
#else
#ifdef _MSC_VER
        err = porting_binder::fcntl_binder( mProcess->mDriverFD, BINDER_WRITE_READ, &bwr );
#else
        err = INVALID_OPERATION;
#endif
#endif
        if (mProcess->mDriverFD < 0) {
            err = -EBADF;
        }
        IF_LOG_COMMANDS() {
            alog.setSourceLocation(__FILE__, __LINE__);
            alog << "Finished read/write, write size = " << mOut.dataSize() << endl;
        }
    } while (err == -EINTR);

    IF_LOG_COMMANDS() {
        alog.setSourceLocation(__FILE__, __LINE__);
        alog << "Our err: " << (void*)(intptr_t)err << ", write consumed: "
            << bwr.write_consumed << " (of " << mOut.dataSize()
                        << "), read consumed: " << bwr.read_consumed << endl;
    }

    if (err >= NO_ERROR) {
        if (bwr.write_consumed > 0) {
            if (bwr.write_consumed < mOut.dataSize())
            {
                LOG_ALWAYS_FATAL("Driver did not consume write buffer. "
                                 "err: %s consumed: %zu of %zu",
                                 statusToString(err).c_str(),
                                 (size_t)bwr.write_consumed,
                                 mOut.dataSize());
            }
            else {
                mOut.setDataSize(0);
                processPostWriteDerefs();
            }
        }
        if (bwr.read_consumed > 0) {
            mIn.setDataSize(bwr.read_consumed);
            mIn.setDataPosition(0);
        }
        IF_LOG_COMMANDS() {
            TextOutput::Bundle _b(alog);
            _b.setSourceLocation(__FILE__, __LINE__);
            alog << "Remaining data size: " << mOut.dataSize() << endl;
            alog << "Received commands from driver: " << indent;
            const void* cmds = mIn.data();
            const void* end = mIn.data() + mIn.dataSize();
            alog << HexDump(cmds, mIn.dataSize()) << endl;
            while (cmds < end) cmds = printReturnCommand(alog, cmds);
            alog << dedent;
        }
        return NO_ERROR;
    }
    return err;
}

#ifdef _MSC_VER
status_t IPCThreadState::writeTransactionData(int32_t cmd, uint32_t binderFlags,
    int32_t handle, uint32_t code, const Parcel& data, status_t* statusBuffer, binder_transaction_data* a_tr)
#else
status_t IPCThreadState::writeTransactionData(int32_t cmd, uint32_t binderFlags,
    int32_t handle, uint32_t code, const Parcel& data, status_t* statusBuffer)
#endif
{
    binder_transaction_data tr;

    tr.target.binder_target_ptr = 0; /* Don't pass uninitialized stack data to a remote process */
    tr.target.binder_handle = handle;
    tr.code = code;
    tr.flags = binderFlags;
    tr.binder_transaction_cookie = 0;
    tr.sender_pid = getpid();
    tr.sender_euid = getpid();

#ifdef _MSC_VER
    tr.is_aidl_transaction = true;
    if( a_tr )
    {
        tr = *a_tr;
        tr.target.binder_handle = handle;
        tr.code = code;
        tr.flags = binderFlags;
    }
    else
    {
        memset( tr.service_name, 0x00, MAX_SERVICE_NAME_SIZE );
        memset( tr.source_connection_name, 0x00, MAX_CONNECTION_NAME_SIZE );
        // 0 is for service manager and -1 is for reply message
        if( handle != 0 && handle != -1 )
        {
            std::string service_name;
            std::string connection_name;
            ipc_connection_token_mgr::get_instance().find_remote_service_by_id
            ( handle, service_name, connection_name );
            service_name.push_back( 0x00 );
            if( ( service_name.size() < MAX_SERVICE_NAME_SIZE ) &&
                ( !service_name.empty() ) )
            {
                strcpy( tr.service_name, service_name.c_str() );
            }
            else
            {
                ALOGE( "service name: %s. it can not longer than %d. or it is empty.",
                    service_name.c_str(), MAX_SERVICE_NAME_SIZE );
            }
        }

        std::string local_connection_name = ipc_connection_token_mgr::get_instance()
            .get_local_connection_name();
        local_connection_name.push_back( 0x00 );
        if( ( local_connection_name.size() < MAX_CONNECTION_NAME_SIZE ) &&
            ( !local_connection_name.empty() ) )
        {
            strcpy( tr.source_connection_name, local_connection_name.c_str() );
        }
        else
        {
            ALOGE( "connection name: %s. it can not longer than %d. or it is empty.",
                local_connection_name.c_str(), MAX_CONNECTION_NAME_SIZE );
        }
    }
#endif

    const status_t err = data.errorCheck();
    if (err == NO_ERROR) {
        tr.data_size = data.ipcDataSize();
        tr.data.ptr.buffer = data.ipcData();
        tr.offsets_size = data.ipcObjectsCount()*sizeof(binder_size_t);
        tr.data.ptr.offsets = data.ipcObjects();
    } else if (statusBuffer) {
        tr.flags |= TF_STATUS_CODE;
        *statusBuffer = err;
        tr.data_size = sizeof(status_t);
        tr.data.ptr.buffer = reinterpret_cast<uintptr_t>(statusBuffer);
        tr.offsets_size = 0;
        tr.data.ptr.offsets = 0;
    } else {
        return (mLastError = err);
    }

    mOut.setDataPosition(0);
    mOut.writeInt32(cmd);
    mOut.write(&tr, sizeof(tr));

    return NO_ERROR;
}

#ifdef _MSC_VER
std::string the_context_object_service_name;
std::recursive_mutex the_context_mutex;
#endif

sp<BBinder> the_context_object;

void IPCThreadState::setTheContextObject(const sp<BBinder>& obj)
{
#ifdef _MSC_VER
    std::lock_guard<std::recursive_mutex> locker( the_context_mutex );
#endif
    the_context_object = obj;
}

status_t IPCThreadState::executeCommand(int32_t cmd)
{
    BBinder* obj;
    RefBase::weakref_type* refs;
    status_t result = NO_ERROR;
    switch ((uint32_t)cmd) {
    case BR_ERROR:
        result = mIn.readInt32();
        break;

    case BR_OK:
        break;

    case BR_ACQUIRE:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();
        ALOG_ASSERT(refs->refBase() == obj,
                   "BR_ACQUIRE: object %p does not match cookie %p (expected %p)",
                   refs, obj, refs->refBase());
        obj->incStrong(mProcess.get());
        IF_LOG_REMOTEREFS() {
            LOG_REMOTEREFS("BR_ACQUIRE from driver on %p", obj);
            obj->printRefs();
        }
        mOut.writeInt32(BC_ACQUIRE_DONE);
        mOut.writePointer((uintptr_t)refs);
        mOut.writePointer((uintptr_t)obj);
        break;

    case BR_RELEASE:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();
        ALOG_ASSERT(refs->refBase() == obj,
                   "BR_RELEASE: object %p does not match cookie %p (expected %p)",
                   refs, obj, refs->refBase());
        IF_LOG_REMOTEREFS() {
            LOG_REMOTEREFS("BR_RELEASE from driver on %p", obj);
            obj->printRefs();
        }
        mPendingStrongDerefs.push(obj);
        break;

    case BR_INCREFS:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();
        refs->incWeak(mProcess.get());
        mOut.writeInt32(BC_INCREFS_DONE);
        mOut.writePointer((uintptr_t)refs);
        mOut.writePointer((uintptr_t)obj);
        break;

    case BR_DECREFS:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        obj = (BBinder*)mIn.readPointer(); // consume
        // NOTE: This assertion is not valid, because the object may no
        // longer exist (thus the (BBinder*)cast above resulting in a different
        // memory address).
        //ALOG_ASSERT(refs->refBase() == obj,
        //           "BR_DECREFS: object %p does not match cookie %p (expected %p)",
        //           refs, obj, refs->refBase());
        mPendingWeakDerefs.push(refs);
        break;

    case BR_ATTEMPT_ACQUIRE:
        refs = (RefBase::weakref_type*)mIn.readPointer();
        obj = (BBinder*)mIn.readPointer();

        {
            const bool success = refs->attemptIncStrong(mProcess.get());
            ALOG_ASSERT(success && refs->refBase() == obj,
                       "BR_ATTEMPT_ACQUIRE: object %p does not match cookie %p (expected %p)",
                       refs, obj, refs->refBase());

            mOut.writeInt32(BC_ACQUIRE_RESULT);
            mOut.writeInt32((int32_t)success);
        }
        break;

    case BR_TRANSACTION_SEC_CTX:
    case BR_TRANSACTION:
        {
            binder_transaction_data_secctx tr_secctx;
            binder_transaction_data& tr = tr_secctx.transaction_data;

            if (cmd == (int) BR_TRANSACTION_SEC_CTX) {
                result = mIn.read(&tr_secctx, sizeof(tr_secctx));
            } else {
                result = mIn.read(&tr, sizeof(tr));
                tr_secctx.secctx = 0;
            }

            ALOG_ASSERT(result == NO_ERROR,
                "Not enough command data for brTRANSACTION");
            if (result != NO_ERROR) break;

            Parcel buffer;
            buffer.ipcSetDataReference(
                reinterpret_cast<const uint8_t*>(tr.data.ptr.buffer),
                tr.data_size,
                reinterpret_cast<const binder_size_t*>(tr.data.ptr.offsets),
                tr.offsets_size/sizeof(binder_size_t), freeBuffer);

            const void* origServingStackPointer = mServingStackPointer;
            mServingStackPointer = 0;// __builtin_frame_address(0);

            const pid_t origPid = mCallingPid;
            const char* origSid = mCallingSid;
            const uid_t origUid = mCallingUid;
            const int32_t origStrictModePolicy = mStrictModePolicy;
            const int32_t origTransactionBinderFlags = mLastTransactionBinderFlags;
            const int32_t origWorkSource = mWorkSource;
            const bool origPropagateWorkSet = mPropagateWorkSource;
            // Calling work source will be set by Parcel#enforceInterface. Parcel#enforceInterface
            // is only guaranteed to be called for AIDL-generated stubs so we reset the work source
            // here to never propagate it.
            clearCallingWorkSource();
            clearPropagateWorkSource();

            mCallingPid = tr.sender_pid;
            mCallingSid = reinterpret_cast<const char*>(tr_secctx.secctx);
            mCallingUid = tr.sender_euid;
            mLastTransactionBinderFlags = tr.flags;

            // ALOGI(">>>> TRANSACT from pid %d sid %s uid %d\n", mCallingPid,
            //    (mCallingSid ? mCallingSid : "<N/A>"), mCallingUid);

            Parcel reply;
            status_t error;
            IF_LOG_TRANSACTIONS() {
                TextOutput::Bundle _b(alog);
                _b.setSourceLocation(__FILE__, __LINE__);
                alog << "BR_TRANSACTION thr " << (void*)gettid()
                    << " / obj " << tr.target.binder_target_ptr << " / code "
                    << TypeCode(tr.code) << ": " << indent << buffer
                    << dedent << endl
                    << "Data addr = "
                    << reinterpret_cast<const uint8_t*>(tr.data.ptr.buffer)
                    << ", offsets addr="
                    << reinterpret_cast<const size_t*>(tr.data.ptr.offsets) << endl;
            }
            if (tr.target.binder_target_ptr) {
                // We only have a weak reference on the target object, so we must first try to
                // safely acquire a strong reference before doing anything else with it.
                if (reinterpret_cast<RefBase::weakref_type*>(
                        tr.target.binder_target_ptr)->attemptIncStrong(this)) {
                    error = reinterpret_cast<BBinder*>(tr.binder_transaction_cookie)->transact(tr.code, buffer,
                            &reply, tr.flags);
                    reinterpret_cast<BBinder*>(tr.binder_transaction_cookie)->decStrong(this);
                } else {
                    error = UNKNOWN_TRANSACTION;
                }

            } else {
#ifdef _MSC_VER
                sp<BBinder> context_object;
                std::unique_lock<std::recursive_mutex> lcker( the_context_mutex );
                if( the_context_object_service_name == tr.service_name )
                {
                    context_object = the_context_object;
                }
                else
                {
                    routeContextObject( tr.service_name );
                    context_object = the_context_object;
                }

                lcker.unlock();
                if( context_object )
                {
                    error = context_object->transact( tr.code, buffer, &reply, tr.flags );
                }
                else
                {
                    ALOGE( "No such service name: %s", tr.service_name );
                }
#else
                error = the_context_object->transact( tr.code, buffer, &reply, tr.flags );
#endif
            }

            //ALOGI("<<<< TRANSACT from pid %d restore pid %d sid %s uid %d\n",
            //     mCallingPid, origPid, (origSid ? origSid : "<N/A>"), origUid);

            if ((tr.flags & TF_ONE_WAY) == 0) {
                LOG_ONEWAY("Sending reply to %d!", mCallingPid);
                if (error < NO_ERROR) reply.setError(error);

                constexpr uint32_t kForwardReplyFlags = TF_CLEAR_BUF;
#ifdef _MSC_VER
                sendReply(reply, (tr.flags & kForwardReplyFlags), &tr);
#else
                sendReply( reply, ( tr.flags& kForwardReplyFlags ) );
#endif
            } else {
                if (error != OK) {
                    alog.setSourceLocation(__FILE__, __LINE__);
                    alog << "oneway function results for code " << tr.code
                         << " on binder at "
                         << reinterpret_cast<void*>(tr.target.binder_target_ptr)
                         << " will be dropped but finished with status "
                         << statusToString(error);

                    // ideally we could log this even when error == OK, but it
                    // causes too much logspam because some manually-written
                    // interfaces have clients that call methods which always
                    // write results, sometimes as oneway methods.
                    if (reply.dataSize() != 0) {
                         alog.setSourceLocation(__FILE__, __LINE__);
                         alog << " and reply parcel size " << reply.dataSize();
                    }

                    alog << endl;
                }
                LOG_ONEWAY("NOT sending reply to %d!", mCallingPid);
            }

            mServingStackPointer = origServingStackPointer;
            mCallingPid = origPid;
            mCallingSid = origSid;
            mCallingUid = origUid;
            mStrictModePolicy = origStrictModePolicy;
            mLastTransactionBinderFlags = origTransactionBinderFlags;
            mWorkSource = origWorkSource;
            mPropagateWorkSource = origPropagateWorkSet;

            IF_LOG_TRANSACTIONS() {
                TextOutput::Bundle _b(alog);
                _b.setSourceLocation(__FILE__, __LINE__);
                alog << "BC_REPLY thr " << (void*)gettid() << " / obj "
                    << tr.target.binder_target_ptr << ": " << indent << reply << dedent << endl;
            }

        }
        break;

    case BR_DEAD_BINDER:
        {
            BpBinder *proxy = (BpBinder*)mIn.readPointer();
            proxy->sendObituary();
            mOut.writeInt32(BC_DEAD_BINDER_DONE);
            mOut.writePointer((uintptr_t)proxy);
        } break;

    case BR_CLEAR_DEATH_NOTIFICATION_DONE:
        {
            BpBinder *proxy = (BpBinder*)mIn.readPointer();
            proxy->getWeakRefs()->decWeak(proxy);
        } break;

    case BR_FINISHED:
        result = TIMED_OUT;
        break;

    case BR_NOOP:
        break;

    case BR_SPAWN_LOOPER:
        mProcess->spawnPooledThread(false);
        break;

    default:
        ALOGE("*** BAD COMMAND %d received from Binder driver\n", cmd);
        result = UNKNOWN_ERROR;
#ifdef _MSC_VER
        porting_binder::debug_invoke();
#endif
        break;
    }

    if (result != NO_ERROR) {
        mLastError = result;
    }

    return result;
}

#ifdef _MSC_VER
void IPCThreadState::routeContextObject( std::string a_service_name )
{
    sp<RefBase> service = ipc_connection_token_mgr::get_instance().get_local_service( a_service_name );
    if( !service )
    {
        ALOGE( "no such service named %s", a_service_name.c_str() );
    }

    BBinder* p = dynamic_cast< BBinder* >( service.get() );
    if( p )
    {
        sp<BBinder> context_object;
        context_object.force_set( p );
        std::lock_guard<std::recursive_mutex> lcker( the_context_mutex );
        the_context_object_service_name = a_service_name;
        setTheContextObject( context_object );
    }
    else
    {
        ALOGE( "cannot cast to BBinder pointer!" );
    }
}
#endif

const void* IPCThreadState::getServingStackPointer() const {
     return mServingStackPointer;
}

void IPCThreadState::threadDestructor(void *st)
{
        IPCThreadState* const self = static_cast<IPCThreadState*>(st);
        if (self) {
                self->flushCommands();
#if defined(__ANDROID__) || defined(_MSC_VER)
        if (self->mProcess->mDriverFD >= 0) {
            porting_binder::fcntl_binder(self->mProcess->mDriverFD, BINDER_THREAD_EXIT, uint32_t(0));
        }
#endif
                delete self;
        }
}

status_t IPCThreadState::getProcessFreezeInfo(pid_t pid, uint32_t *sync_received,
                                              uint32_t *async_received)
{
    int ret = 0;
    binder_frozen_status_info info = {};
    info.pid = pid;

#if defined(__ANDROID__) || defined(_MSC_VER)
    if (porting_binder::fcntl_binder(self()->mProcess->mDriverFD, BINDER_GET_FROZEN_INFO, &info) < 0)
        ret = -errno;
#endif
    *sync_received = info.sync_recv;
    *async_received = info.async_recv;

    return ret;
}

status_t IPCThreadState::freeze(pid_t pid, bool enable, uint32_t timeout_ms) {
    struct binder_freeze_info info;
    int ret = 0;

    info.pid = pid;
    info.enable = enable;
    info.timeout_ms = timeout_ms;


#if defined(__ANDROID__) || defined(_MSC_VER)
    if (porting_binder::fcntl_binder(self()->mProcess->mDriverFD, BINDER_FREEZE, &info) < 0)
        ret = -errno;
#endif

    //
    // ret==-EAGAIN indicates that transactions have not drained.
    // Call again to poll for completion.
    //
    return ret;
}

void IPCThreadState::logExtendedError() {
    struct binder_extended_error ee;
    memset(&ee,0x00,sizeof(binder_extended_error) );
    ee.command = 0;

    if (!ProcessState::isDriverFeatureEnabled(ProcessState::DriverFeature::EXTENDED_ERROR))
        return;

#if defined(__ANDROID__) || defined(_MSC_VER)
    if (porting_binder::fcntl_binder(self()->mProcess->mDriverFD, BINDER_GET_EXTENDED_ERROR, &ee) < 0) {
        ALOGE("Failed to get extended error: %s", strerror(errno));
        return;
    }
#endif

    ALOGE_IF(ee.command != 0, "Binder transaction failure: %d/%d/%d",
             ee.id, ee.command, ee.param);
}

void IPCThreadState::freeBuffer(const uint8_t* data, size_t /*dataSize*/,
                                const binder_size_t* /*objects*/, size_t /*objectsSize*/) {
    //ALOGI("Freeing parcel %p", &parcel);

    IF_LOG_COMMANDS() {
        alog.setSourceLocation(__FILE__, __LINE__);
        alog << "Writing BC_FREE_BUFFER for " << data << endl;
    }
    ALOG_ASSERT(data != NULL, "Called with NULL data");
    IPCThreadState* state = self();
#ifdef _MSC_VER
    state->mOut.setDataPosition( 0 );
    state->mOut.setDataSize( 0 );
#endif
    state->mOut.writeInt32(BC_FREE_BUFFER);
    state->mOut.writePointer((uintptr_t)data);
    state->flushIfNeeded();

}

} // namespace android
