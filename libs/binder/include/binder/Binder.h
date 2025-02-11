/*
 * Copyright (C) 2008 The Android Open Source Project
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

#pragma once

#include <atomic>
#include <stdint.h>
#include <binder/Common.h>
#include <binder/IBinder.h>

#include <binder/libbinder_export.h>

        // ---------------------------------------------------------------------------
namespace android {

namespace internal {
class Stability;
}

<<<<<<< HEAD
class LIBBINDER_EXPORT BBinder : public IBinder {
=======
class BBinder : public IBinder {
>>>>>>> d3fb93fb73
public:
    LIBBINDER_EXPORTED BBinder();

    LIBBINDER_EXPORTED virtual const String16& getInterfaceDescriptor() const;
    LIBBINDER_EXPORTED virtual bool isBinderAlive() const;
    LIBBINDER_EXPORTED virtual status_t pingBinder();
    LIBBINDER_EXPORTED virtual status_t dump(int fd, const Vector<String16>& args);

    // NOLINTNEXTLINE(google-default-arguments)
    LIBBINDER_EXPORTED virtual status_t transact(uint32_t code, const Parcel& data, Parcel* reply,
                                                 uint32_t flags = 0) final;

    // NOLINTNEXTLINE(google-default-arguments)
    LIBBINDER_EXPORTED virtual status_t linkToDeath(const sp<DeathRecipient>& recipient,
                                                    void* cookie = nullptr, uint32_t flags = 0);

    // NOLINTNEXTLINE(google-default-arguments)
    LIBBINDER_EXPORTED virtual status_t unlinkToDeath(const wp<DeathRecipient>& recipient,
                                                      void* cookie = nullptr, uint32_t flags = 0,
                                                      wp<DeathRecipient>* outRecipient = nullptr);

    LIBBINDER_EXPORTED virtual void* attachObject(const void* objectID, void* object,
                                                  void* cleanupCookie,
                                                  object_cleanup_func func) final;
    LIBBINDER_EXPORTED virtual void* findObject(const void* objectID) const final;
    LIBBINDER_EXPORTED virtual void* detachObject(const void* objectID) final;
    LIBBINDER_EXPORTED void withLock(const std::function<void()>& doWithLock);
    LIBBINDER_EXPORTED sp<IBinder> lookupOrCreateWeak(const void* objectID,
                                                      IBinder::object_make_func make,
                                                      const void* makeArgs);

    LIBBINDER_EXPORTED virtual BBinder* localBinder();

    LIBBINDER_EXPORTED bool isRequestingSid();
    // This must be called before the object is sent to another process. Not thread safe.
    LIBBINDER_EXPORTED void setRequestingSid(bool requestSid);

    LIBBINDER_EXPORTED sp<IBinder> getExtension();
    // This must be called before the object is sent to another process. Not thread safe.
    LIBBINDER_EXPORTED void setExtension(const sp<IBinder>& extension);

    // This must be called before the object is sent to another process. Not thread safe.
    //
    // This function will abort if improper parameters are set. This is like
    // sched_setscheduler. However, it sets the minimum scheduling policy
    // only for the duration that this specific binder object is handling the
    // call in a threadpool. By default, this API is set to SCHED_NORMAL/0. In
    // this case, the scheduling priority will not actually be modified from
    // binder defaults. See also IPCThreadState::disableBackgroundScheduling.
    //
    // Appropriate values are:
    // SCHED_NORMAL: -20 <= priority <= 19
    // SCHED_RR/SCHED_FIFO: 1 <= priority <= 99
    LIBBINDER_EXPORTED void setMinSchedulerPolicy(int policy, int priority);
    LIBBINDER_EXPORTED int getMinSchedulerPolicy();
    LIBBINDER_EXPORTED int getMinSchedulerPriority();

    // Whether realtime scheduling policies are inherited.
    LIBBINDER_EXPORTED bool isInheritRt();
    // This must be called before the object is sent to another process. Not thread safe.
    LIBBINDER_EXPORTED void setInheritRt(bool inheritRt);

    LIBBINDER_EXPORTED pid_t getDebugPid();

    // Whether this binder has been sent to another process.
    LIBBINDER_EXPORTED bool wasParceled();
    // Consider this binder as parceled (setup/init-related calls should no
    // longer by called. This is automatically set by when this binder is sent
    // to another process.
    LIBBINDER_EXPORTED void setParceled();

<<<<<<< HEAD
    [[nodiscard]] status_t setRpcClientDebug(android::base::unique_fd socketFd,
                                             const sp<IBinder>& keepAliveBinder);
=======
    [[nodiscard]] LIBBINDER_EXPORTED status_t setRpcClientDebug(binder::unique_fd clientFd,
                                                                const sp<IBinder>& keepAliveBinder);
>>>>>>> d3fb93fb73

protected:
    LIBBINDER_EXPORTED virtual ~BBinder();

    // NOLINTNEXTLINE(google-default-arguments)
    LIBBINDER_EXPORTED virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                                                   uint32_t flags = 0);

private:
                        BBinder(const BBinder& o);
            BBinder&    operator=(const BBinder& o);

    class RpcServerLink;
    class Extras;

    Extras*             getOrCreateExtras();

    [[nodiscard]] status_t setRpcClientDebug(const Parcel& data);
    void removeRpcServerLink(const sp<RpcServerLink>& link);
    [[nodiscard]] status_t startRecordingTransactions(const Parcel& data);
    [[nodiscard]] status_t stopRecordingTransactions();

    std::atomic<Extras*> mExtras;

    friend ::android::internal::Stability;
    int16_t mStability;
    bool mParceled;
    bool mRecordingOn;

#ifdef __LP64__
    int32_t mReserved1;
#endif
};

// ---------------------------------------------------------------------------

<<<<<<< HEAD
class LIBBINDER_EXPORT BpRefBase : public virtual RefBase {
=======
class BpRefBase : public virtual RefBase {
>>>>>>> d3fb93fb73
protected:
    LIBBINDER_EXPORTED explicit BpRefBase(const sp<IBinder>& o);
    LIBBINDER_EXPORTED virtual ~BpRefBase();
    LIBBINDER_EXPORTED virtual void onFirstRef();
    LIBBINDER_EXPORTED virtual void onLastStrongRef(const void* id);
    LIBBINDER_EXPORTED virtual bool onIncStrongAttempted(uint32_t flags, const void* id);

    LIBBINDER_EXPORTED inline IBinder* remote() const { return mRemote; }
    LIBBINDER_EXPORTED inline sp<IBinder> remoteStrong() const {
        return sp<IBinder>::fromExisting(mRemote);
    }

private:
                            BpRefBase(const BpRefBase& o);
    BpRefBase&              operator=(const BpRefBase& o);

    IBinder* const          mRemote;
    RefBase::weakref_type*  mRefs;
    std::atomic<int32_t>    mState;
};

} // namespace android

// ---------------------------------------------------------------------------
