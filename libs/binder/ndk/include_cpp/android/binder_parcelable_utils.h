/*
 * Copyright (C) 2020 The Android Open Source Project
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

/**
 * @addtogroup NdkBinder
 * @{
 */

/**
 * @file binder_parcelable_utils.h
 * @brief Helper for parcelable.
 */

#pragma once
#include <android/binder_parcel_utils.h>
#include <optional>

namespace ndk {
// Also see Parcelable.h in libbinder.
typedef int32_t parcelable_stability_t;
enum {
    STABILITY_LOCAL,
    STABILITY_VINTF,  // corresponds to @VintfStability
};
#define RETURN_ON_FAILURE(expr)                   \
    do {                                          \
        binder_status_t _status = (expr);         \
        if (_status != STATUS_OK) return _status; \
    } while (false)

class AParcelableHolder {
   public:
    AParcelableHolder() = delete;
    explicit AParcelableHolder(parcelable_stability_t stability)
        : mParcel(AParcel_create()), mStability(stability) {}

#if __ANDROID_API__ >= 31
    AParcelableHolder(const AParcelableHolder& other)
        : mParcel(AParcel_create()), mStability(other.mStability) {
        // AParcelableHolder has been introduced in 31.
#ifdef __ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__
        if (__builtin_available(android 31, *)) {
#else
        if (__ANDROID_API__ >= 31) {
#endif
            AParcel_appendFrom(other.mParcel.get(), this->mParcel.get(), 0,
                               AParcel_getDataSize(other.mParcel.get()));
        }
    }
#endif

    AParcelableHolder& operator=(const AParcelableHolder& other)
    {
        mParcel = ndk::ScopedAParcel(AParcel_create());
        mStability = other.mStability;
        AParcel_appendFrom(other.mParcel.get(), this->mParcel.get(), 0,
            AParcel_getDataSize(other.mParcel.get()));
        return *this;
    }

    AParcelableHolder(AParcelableHolder&& other) = default;
    virtual ~AParcelableHolder() = default;

    binder_status_t writeToParcel(AParcel* parcel) const {
        RETURN_ON_FAILURE(AParcel_writeInt32(parcel, static_cast<int32_t>(this->mStability)));
#ifdef __ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__
        if (__builtin_available(android 31, *)) {
#else
        if (__ANDROID_API__ >= 31) {
#endif
            int32_t size = AParcel_getDataSize(this->mParcel.get());
            RETURN_ON_FAILURE(AParcel_writeInt32(parcel, size));
        } else {
            return STATUS_INVALID_OPERATION;
        }
#ifdef __ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__
        if (__builtin_available(android 31, *)) {
#else
        if (__ANDROID_API__ >= 31) {
#endif
            int32_t size = AParcel_getDataSize(this->mParcel.get());
            RETURN_ON_FAILURE(AParcel_appendFrom(this->mParcel.get(), parcel, 0, size));
        } else {
            return STATUS_INVALID_OPERATION;
        }
        return STATUS_OK;
    }

    binder_status_t readFromParcel(const AParcel* parcel) {
#ifdef __ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__
        if (__builtin_available(android 31, *)) {
#else
        if (__ANDROID_API__ >= 31) {
#endif
            AParcel_reset(mParcel.get());
        } else {
            return STATUS_INVALID_OPERATION;
        }

        parcelable_stability_t wireStability;
        RETURN_ON_FAILURE(AParcel_readInt32(parcel, &wireStability));
        if (this->mStability != wireStability) {
            return STATUS_BAD_VALUE;
        }

        int32_t dataSize;
        binder_status_t status = AParcel_readInt32(parcel, &dataSize);

        if (status != STATUS_OK || dataSize < 0) {
            return status != STATUS_OK ? status : STATUS_BAD_VALUE;
        }

        int32_t dataStartPos = AParcel_getDataPosition(parcel);

        if (dataStartPos > INT32_MAX - dataSize) {
            return STATUS_BAD_VALUE;
        }

#ifdef __ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__
        if (__builtin_available(android 31, *)) {
#else
        if (__ANDROID_API__ >= 31) {
#endif
            status = AParcel_appendFrom(parcel, mParcel.get(), dataStartPos, dataSize);
        } else {
            status = STATUS_INVALID_OPERATION;
        }
        if (status != STATUS_OK) {
            return status;
        }
        return AParcel_setDataPosition(parcel, dataStartPos + dataSize);
    }

    template <typename T>
    binder_status_t setParcelable(const T& p) {
        if (this->mStability > T::_aidl_stability) {
            return STATUS_BAD_VALUE;
        }
#ifdef __ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__
        if (__builtin_available(android 31, *)) {
#else
        if (__ANDROID_API__ >= 31) {
#endif
            AParcel_reset(mParcel.get());
        } else {
            return STATUS_INVALID_OPERATION;
        }
        AParcel_writeString(mParcel.get(), T::descriptor, strlen(T::descriptor));
        p.writeToParcel(mParcel.get());
        return STATUS_OK;
    }

    template <typename T>
    binder_status_t getParcelable(std::optional<T>* ret) const {
        const std::string parcelableDesc(T::descriptor);
        AParcel_setDataPosition(mParcel.get(), 0);
#ifdef __ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__
        if (__builtin_available(android 31, *)) {
#else
        if (__ANDROID_API__ >= 31) {
#endif
            if (AParcel_getDataSize(mParcel.get()) == 0) {
                *ret = std::nullopt;
                return STATUS_OK;
            }
        } else {
            return STATUS_INVALID_OPERATION;
        }
        std::string parcelableDescInParcel;
        binder_status_t status = AParcel_readString(mParcel.get(), &parcelableDescInParcel);
        if (status != STATUS_OK || parcelableDesc != parcelableDescInParcel) {
            *ret = std::nullopt;
            return status;
        }
        *ret = std::make_optional<T>();
        status = (*ret)->readFromParcel(this->mParcel.get());
        if (status != STATUS_OK) {
            *ret = std::nullopt;
            return status;
        }
        return STATUS_OK;
    }

    void reset() {
#ifdef __ANDROID_UNAVAILABLE_SYMBOLS_ARE_WEAK__
        if (__builtin_available(android 31, *)) {
#else
        if (__ANDROID_API__ >= 31) {
#endif
            AParcel_reset(mParcel.get());
        }
    }

    inline bool operator!=(const AParcelableHolder& rhs) const { return this != &rhs; }
    inline bool operator<(const AParcelableHolder& rhs) const { return this < &rhs; }
    inline bool operator<=(const AParcelableHolder& rhs) const { return this <= &rhs; }
    inline bool operator==(const AParcelableHolder& rhs) const { return this == &rhs; }
    inline bool operator>(const AParcelableHolder& rhs) const { return this > &rhs; }
    inline bool operator>=(const AParcelableHolder& rhs) const { return this >= &rhs; }

   private:
    mutable ndk::ScopedAParcel mParcel;
    parcelable_stability_t mStability;
};

#undef RETURN_ON_FAILURE
}  // namespace ndk

/** @} */
