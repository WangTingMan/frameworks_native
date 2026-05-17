/*
 * This file is auto-generated.  DO NOT MODIFY.
 * Using: out/host/linux-x86/bin/aidl-cpp -dout/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/StatsBootstrapAtomValue.cpp.d --ninja -t --min_sdk_version=platform_apis -Iframeworks/base/core/java/ frameworks/base/core/java/android/os/StatsBootstrapAtomValue.aidl out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out out/soong/.temp/sbox/fb56b790b02b12751e2172e614fa9ca72920f5c8/out/android/os/StatsBootstrapAtomValue.cpp
 *
 * DO NOT CHECK THIS FILE INTO A CODE TREE (e.g. git, etc..).
 * ALWAYS GENERATE THIS FILE FROM UPDATED AIDL COMPILER
 * AS A BUILD INTERMEDIATE ONLY. THIS IS NOT SOURCE CODE.
 */
#pragma once

#include <android/binder_to_string.h>
#include <android/os/StatsBootstrapAtomValue.h>
#include <array>
#include <binder/Enums.h>
#include <binder/Parcel.h>
#include <binder/Status.h>
#include <cassert>
#include <cstdint>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <utils/String16.h>
#include <variant>
#include <vector>

#ifndef __BIONIC__
#define __assert2(a,b,c,d) ((void)0)
#endif

namespace android {
namespace os {
class LIBBINDER_EXPORTED StatsBootstrapAtomValue : public ::android::Parcelable {
public:
  class LIBBINDER_EXPORTED Primitive : public ::android::Parcelable {
  public:
    enum class Tag : int32_t {
      boolValue = 0,
      intValue = 1,
      longValue = 2,
      floatValue = 3,
      stringValue = 4,
      bytesValue = 5,
      stringArrayValue = 6,
    };
    // Expose tag symbols for legacy code
    static const inline Tag boolValue = Tag::boolValue;
    static const inline Tag intValue = Tag::intValue;
    static const inline Tag longValue = Tag::longValue;
    static const inline Tag floatValue = Tag::floatValue;
    static const inline Tag stringValue = Tag::stringValue;
    static const inline Tag bytesValue = Tag::bytesValue;
    static const inline Tag stringArrayValue = Tag::stringArrayValue;

    template<typename _Tp>
    static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, Primitive>;

    Primitive() : _value(std::in_place_index<static_cast<size_t>(boolValue)>, bool(false)) { }

    template <typename _Tp, typename = std::enable_if_t<
        _not_self<_Tp> &&
        std::is_constructible_v<std::variant<bool, int32_t, int64_t, float, ::android::String16, ::std::vector<uint8_t>, ::std::vector<::android::String16>>, _Tp>
      >>
    // NOLINTNEXTLINE(google-explicit-constructor)
    constexpr Primitive(_Tp&& _arg)
        : _value(std::forward<_Tp>(_arg)) {}

    template <size_t _Np, typename... _Tp>
    constexpr explicit Primitive(std::in_place_index_t<_Np>, _Tp&&... _args)
        : _value(std::in_place_index<_Np>, std::forward<_Tp>(_args)...) {}

    template <Tag _tag, typename... _Tp>
    static Primitive make(_Tp&&... _args) {
      return Primitive(std::in_place_index<static_cast<size_t>(_tag)>, std::forward<_Tp>(_args)...);
    }

    template <Tag _tag, typename _Tp, typename... _Up>
    static Primitive make(std::initializer_list<_Tp> _il, _Up&&... _args) {
      return Primitive(std::in_place_index<static_cast<size_t>(_tag)>, std::move(_il), std::forward<_Up>(_args)...);
    }

    Tag getTag() const {
      return static_cast<Tag>(_value.index());
    }

    template <Tag _tag>
    const auto& get() const {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag>
    auto& get() {
      if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
      return std::get<static_cast<size_t>(_tag)>(_value);
    }

    template <Tag _tag, typename... _Tp>
    void set(_Tp&&... _args) {
      _value.emplace<static_cast<size_t>(_tag)>(std::forward<_Tp>(_args)...);
    }

    inline bool operator==(const Primitive& _rhs) const {
      return _value == _rhs._value;
    }
    inline bool operator<(const Primitive& _rhs) const {
      return _value < _rhs._value;
    }
    inline bool operator!=(const Primitive& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const Primitive& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const Primitive& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const Primitive& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.os.StatsBootstrapAtomValue.Primitive");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream os;
      os << "Primitive{";
      switch (getTag()) {
      case boolValue: os << "boolValue: " << ::android::internal::ToString(get<boolValue>()); break;
      case intValue: os << "intValue: " << ::android::internal::ToString(get<intValue>()); break;
      case longValue: os << "longValue: " << ::android::internal::ToString(get<longValue>()); break;
      case floatValue: os << "floatValue: " << ::android::internal::ToString(get<floatValue>()); break;
      case stringValue: os << "stringValue: " << ::android::internal::ToString(get<stringValue>()); break;
      case bytesValue: os << "bytesValue: " << ::android::internal::ToString(get<bytesValue>()); break;
      case stringArrayValue: os << "stringArrayValue: " << ::android::internal::ToString(get<stringArrayValue>()); break;
      }
      os << "}";
      return os.str();
    }
  private:
    std::variant<bool, int32_t, int64_t, float, ::android::String16, ::std::vector<uint8_t>, ::std::vector<::android::String16>> _value;
  };  // class Primitive
  class LIBBINDER_EXPORTED Annotation : public ::android::Parcelable {
  public:
    enum class Id : int8_t {
      NONE = 0,
      IS_UID = 1,
    };
    class LIBBINDER_EXPORTED Primitive : public ::android::Parcelable {
    public:
      enum class Tag : int32_t {
        boolValue = 0,
        intValue = 1,
      };
      // Expose tag symbols for legacy code
      static const inline Tag boolValue = Tag::boolValue;
      static const inline Tag intValue = Tag::intValue;

      template<typename _Tp>
      static constexpr bool _not_self = !std::is_same_v<std::remove_cv_t<std::remove_reference_t<_Tp>>, Primitive>;

      Primitive() : _value(std::in_place_index<static_cast<size_t>(boolValue)>, bool(false)) { }

      template <typename _Tp, typename = std::enable_if_t<
          _not_self<_Tp> &&
          std::is_constructible_v<std::variant<bool, int32_t>, _Tp>
        >>
      // NOLINTNEXTLINE(google-explicit-constructor)
      constexpr Primitive(_Tp&& _arg)
          : _value(std::forward<_Tp>(_arg)) {}

      template <size_t _Np, typename... _Tp>
      constexpr explicit Primitive(std::in_place_index_t<_Np>, _Tp&&... _args)
          : _value(std::in_place_index<_Np>, std::forward<_Tp>(_args)...) {}

      template <Tag _tag, typename... _Tp>
      static Primitive make(_Tp&&... _args) {
        return Primitive(std::in_place_index<static_cast<size_t>(_tag)>, std::forward<_Tp>(_args)...);
      }

      template <Tag _tag, typename _Tp, typename... _Up>
      static Primitive make(std::initializer_list<_Tp> _il, _Up&&... _args) {
        return Primitive(std::in_place_index<static_cast<size_t>(_tag)>, std::move(_il), std::forward<_Up>(_args)...);
      }

      Tag getTag() const {
        return static_cast<Tag>(_value.index());
      }

      template <Tag _tag>
      const auto& get() const {
        if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
        return std::get<static_cast<size_t>(_tag)>(_value);
      }

      template <Tag _tag>
      auto& get() {
        if (getTag() != _tag) { __assert2(__FILE__, __LINE__, __PRETTY_FUNCTION__, "bad access: a wrong tag"); }
        return std::get<static_cast<size_t>(_tag)>(_value);
      }

      template <Tag _tag, typename... _Tp>
      void set(_Tp&&... _args) {
        _value.emplace<static_cast<size_t>(_tag)>(std::forward<_Tp>(_args)...);
      }

      inline bool operator==(const Primitive& _rhs) const {
        return _value == _rhs._value;
      }
      inline bool operator<(const Primitive& _rhs) const {
        return _value < _rhs._value;
      }
      inline bool operator!=(const Primitive& _rhs) const {
        return !(*this == _rhs);
      }
      inline bool operator>(const Primitive& _rhs) const {
        return _rhs < *this;
      }
      inline bool operator>=(const Primitive& _rhs) const {
        return !(*this < _rhs);
      }
      inline bool operator<=(const Primitive& _rhs) const {
        return !(_rhs < *this);
      }

      ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
      ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
      static const ::android::String16& getParcelableDescriptor() {
        [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.os.StatsBootstrapAtomValue.Annotation.Primitive");
        return DESCRIPTOR;
      }
      inline std::string toString() const {
        std::ostringstream os;
        os << "Primitive{";
        switch (getTag()) {
        case boolValue: os << "boolValue: " << ::android::internal::ToString(get<boolValue>()); break;
        case intValue: os << "intValue: " << ::android::internal::ToString(get<intValue>()); break;
        }
        os << "}";
        return os.str();
      }
    private:
      std::variant<bool, int32_t> _value;
    };  // class Primitive
    ::android::os::StatsBootstrapAtomValue::Annotation::Id id = ::android::os::StatsBootstrapAtomValue::Annotation::Id(0);
    ::android::os::StatsBootstrapAtomValue::Annotation::Primitive value;
    inline bool operator==(const Annotation& _rhs) const {
      return std::tie(id, value) == std::tie(_rhs.id, _rhs.value);
    }
    inline bool operator<(const Annotation& _rhs) const {
      return std::tie(id, value) < std::tie(_rhs.id, _rhs.value);
    }
    inline bool operator!=(const Annotation& _rhs) const {
      return !(*this == _rhs);
    }
    inline bool operator>(const Annotation& _rhs) const {
      return _rhs < *this;
    }
    inline bool operator>=(const Annotation& _rhs) const {
      return !(*this < _rhs);
    }
    inline bool operator<=(const Annotation& _rhs) const {
      return !(_rhs < *this);
    }

    ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
    ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
    static const ::android::String16& getParcelableDescriptor() {
      [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.os.StatsBootstrapAtomValue.Annotation");
      return DESCRIPTOR;
    }
    inline std::string toString() const {
      std::ostringstream _aidl_os;
      _aidl_os << "Annotation{";
      _aidl_os << "id: " << ::android::internal::ToString(id);
      _aidl_os << ", value: " << ::android::internal::ToString(value);
      _aidl_os << "}";
      return _aidl_os.str();
    }
  };  // class Annotation
  ::android::os::StatsBootstrapAtomValue::Primitive value;
  ::std::vector<::android::os::StatsBootstrapAtomValue::Annotation> annotations;
  inline bool operator==(const StatsBootstrapAtomValue& _rhs) const {
    return std::tie(value, annotations) == std::tie(_rhs.value, _rhs.annotations);
  }
  inline bool operator<(const StatsBootstrapAtomValue& _rhs) const {
    return std::tie(value, annotations) < std::tie(_rhs.value, _rhs.annotations);
  }
  inline bool operator!=(const StatsBootstrapAtomValue& _rhs) const {
    return !(*this == _rhs);
  }
  inline bool operator>(const StatsBootstrapAtomValue& _rhs) const {
    return _rhs < *this;
  }
  inline bool operator>=(const StatsBootstrapAtomValue& _rhs) const {
    return !(*this < _rhs);
  }
  inline bool operator<=(const StatsBootstrapAtomValue& _rhs) const {
    return !(_rhs < *this);
  }

  ::android::status_t readFromParcel(const ::android::Parcel* _aidl_parcel) final;
  ::android::status_t writeToParcel(::android::Parcel* _aidl_parcel) const final;
  static const ::android::String16& getParcelableDescriptor() {
    [[clang::no_destroy]] static const ::android::StaticString16 DESCRIPTOR (u"android.os.StatsBootstrapAtomValue");
    return DESCRIPTOR;
  }
  inline std::string toString() const {
    std::ostringstream _aidl_os;
    _aidl_os << "StatsBootstrapAtomValue{";
    _aidl_os << "value: " << ::android::internal::ToString(value);
    _aidl_os << ", annotations: " << ::android::internal::ToString(annotations);
    _aidl_os << "}";
    return _aidl_os.str();
  }
};  // class StatsBootstrapAtomValue
}  // namespace os
}  // namespace android
namespace android {
namespace os {
[[nodiscard]] static inline std::string toString(StatsBootstrapAtomValue::Primitive::Tag val) {
  switch(val) {
  case StatsBootstrapAtomValue::Primitive::Tag::boolValue:
    return "boolValue";
  case StatsBootstrapAtomValue::Primitive::Tag::intValue:
    return "intValue";
  case StatsBootstrapAtomValue::Primitive::Tag::longValue:
    return "longValue";
  case StatsBootstrapAtomValue::Primitive::Tag::floatValue:
    return "floatValue";
  case StatsBootstrapAtomValue::Primitive::Tag::stringValue:
    return "stringValue";
  case StatsBootstrapAtomValue::Primitive::Tag::bytesValue:
    return "bytesValue";
  case StatsBootstrapAtomValue::Primitive::Tag::stringArrayValue:
    return "stringArrayValue";
  default:
    return std::to_string(static_cast<int32_t>(val));
  }
}
}  // namespace os
}  // namespace android
namespace android {
namespace internal {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template <>
constexpr inline std::array<::android::os::StatsBootstrapAtomValue::Primitive::Tag, 7> enum_values<::android::os::StatsBootstrapAtomValue::Primitive::Tag> = {
  ::android::os::StatsBootstrapAtomValue::Primitive::Tag::boolValue,
  ::android::os::StatsBootstrapAtomValue::Primitive::Tag::intValue,
  ::android::os::StatsBootstrapAtomValue::Primitive::Tag::longValue,
  ::android::os::StatsBootstrapAtomValue::Primitive::Tag::floatValue,
  ::android::os::StatsBootstrapAtomValue::Primitive::Tag::stringValue,
  ::android::os::StatsBootstrapAtomValue::Primitive::Tag::bytesValue,
  ::android::os::StatsBootstrapAtomValue::Primitive::Tag::stringArrayValue,
};
#pragma clang diagnostic pop
}  // namespace internal
}  // namespace android
namespace android {
namespace os {
[[nodiscard]] static inline std::string toString(StatsBootstrapAtomValue::Annotation::Id val) {
  switch(val) {
  case StatsBootstrapAtomValue::Annotation::Id::NONE:
    return "NONE";
  case StatsBootstrapAtomValue::Annotation::Id::IS_UID:
    return "IS_UID";
  default:
    return std::to_string(static_cast<int8_t>(val));
  }
}
}  // namespace os
}  // namespace android
namespace android {
namespace internal {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template <>
constexpr inline std::array<::android::os::StatsBootstrapAtomValue::Annotation::Id, 2> enum_values<::android::os::StatsBootstrapAtomValue::Annotation::Id> = {
  ::android::os::StatsBootstrapAtomValue::Annotation::Id::NONE,
  ::android::os::StatsBootstrapAtomValue::Annotation::Id::IS_UID,
};
#pragma clang diagnostic pop
}  // namespace internal
}  // namespace android
namespace android {
namespace os {
[[nodiscard]] static inline std::string toString(StatsBootstrapAtomValue::Annotation::Primitive::Tag val) {
  switch(val) {
  case StatsBootstrapAtomValue::Annotation::Primitive::Tag::boolValue:
    return "boolValue";
  case StatsBootstrapAtomValue::Annotation::Primitive::Tag::intValue:
    return "intValue";
  default:
    return std::to_string(static_cast<int32_t>(val));
  }
}
}  // namespace os
}  // namespace android
namespace android {
namespace internal {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc++17-extensions"
template <>
constexpr inline std::array<::android::os::StatsBootstrapAtomValue::Annotation::Primitive::Tag, 2> enum_values<::android::os::StatsBootstrapAtomValue::Annotation::Primitive::Tag> = {
  ::android::os::StatsBootstrapAtomValue::Annotation::Primitive::Tag::boolValue,
  ::android::os::StatsBootstrapAtomValue::Annotation::Primitive::Tag::intValue,
};
#pragma clang diagnostic pop
}  // namespace internal
}  // namespace android
