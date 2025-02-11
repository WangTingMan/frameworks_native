/*
 * Copyright (C) 2006 The Android Open Source Project
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

#include <binder/Common.h>
#include <utils/Errors.h>
#include <utils/String8.h>

#include <stdint.h>
#include <string.h>
#include <sstream>
#include <binder/libbinder_export.h>

// ---------------------------------------------------------------------------
namespace android {

<<<<<<< HEAD
class LIBBINDER_EXPORT TextOutput
{
=======
class LIBBINDER_EXPORTED TextOutput {
>>>>>>> d3fb93fb73
public:
                        TextOutput();
    virtual             ~TextOutput();
    
    virtual status_t    print(const char* txt, size_t len) = 0;
    virtual void        moveIndent(int delta) = 0;
    
    class Bundle {
    public:
        inline explicit Bundle(TextOutput& to) : mTO(to) { to.pushBundle(); }
        inline ~Bundle() { mTO.popBundle(); }
        inline void setSourceLocation(const char* file, uint32_t line) {
            mTO.setSourceLocation(file, line);
        }
    private:
        TextOutput&     mTO;
    };
    
    virtual void        pushBundle() = 0;
    virtual void popBundle() = 0;

    inline void setSourceLocation(const char* file, uint32_t line) {
        mSourceFile = file;
        mSourceLine = line;
    }

    const char* mSourceFile = nullptr;
    uint32_t mSourceLine = 0;
};

// ---------------------------------------------------------------------------

// DO NOT USE: prefer libutils/libbase logs, which don't require static data to
// be allocated.
// Text output stream for printing to the log (via utils/Log.h).
<<<<<<< HEAD
LIBBINDER_EXPORT extern TextOutput& alog;
=======
extern LIBBINDER_EXPORTED TextOutput& alog;
>>>>>>> d3fb93fb73

// DO NOT USE: prefer libutils/libbase logs, which don't require static data to
// be allocated.
// Text output stream for printing to stdout.
<<<<<<< HEAD
LIBBINDER_EXPORT extern TextOutput& aout;
=======
extern LIBBINDER_EXPORTED TextOutput& aout;
>>>>>>> d3fb93fb73

// DO NOT USE: prefer libutils/libbase logs, which don't require static data to
// be allocated.
// Text output stream for printing to stderr.
<<<<<<< HEAD
LIBBINDER_EXPORT extern TextOutput& aerr;
=======
extern LIBBINDER_EXPORTED TextOutput& aerr;
>>>>>>> d3fb93fb73

typedef TextOutput& (*TextOutputManipFunc)(TextOutput&);

TextOutput& endl(TextOutput& to);
TextOutput& indent(TextOutput& to);
TextOutput& dedent(TextOutput& to);

template<typename T>
TextOutput& operator<<(TextOutput& to, const T& val)
{
    std::stringstream strbuf;
    strbuf << val;
    std::string str = strbuf.str();
    to.print(str.c_str(), str.size());
    return to;
}

LIBBINDER_EXPORTED TextOutput& operator<<(TextOutput& to, TextOutputManipFunc func);

<<<<<<< HEAD
class LIBBINDER_EXPORT TypeCode
{
=======
class LIBBINDER_EXPORTED TypeCode {
>>>>>>> d3fb93fb73
public:
    inline explicit TypeCode(uint32_t code);
    inline ~TypeCode();

    inline uint32_t typeCode() const;

private:
    uint32_t mCode;
};

LIBBINDER_EXPORTED std::ostream& operator<<(std::ostream& to, const TypeCode& val);

<<<<<<< HEAD
class LIBBINDER_EXPORT HexDump
{
=======
class LIBBINDER_EXPORTED HexDump {
>>>>>>> d3fb93fb73
public:
    HexDump(const void *buf, size_t size, size_t bytesPerLine=16);
    inline ~HexDump();
    
    inline HexDump& setBytesPerLine(size_t bytesPerLine);
    inline HexDump& setSingleLineCutoff(int32_t bytes);
    inline HexDump& setAlignment(size_t alignment);
    inline HexDump& setCArrayStyle(bool enabled);
    
    inline const void* buffer() const;
    inline size_t size() const;
    inline size_t bytesPerLine() const;
    inline int32_t singleLineCutoff() const;
    inline size_t alignment() const;
    inline bool carrayStyle() const;

private:
    const void* mBuffer;
    size_t mSize;
    size_t mBytesPerLine;
    int32_t mSingleLineCutoff;
    size_t mAlignment;
    bool mCArrayStyle;
};

LIBBINDER_EXPORTED std::ostream& operator<<(std::ostream& to, const HexDump& val);
inline TextOutput& operator<<(TextOutput& to,
                              decltype(std::endl<char,
                                       std::char_traits<char>>)
                              /*val*/) {
    endl(to);
    return to;
}

inline TextOutput& operator<<(TextOutput& to, const char &c)
{
    to.print(&c, 1);
    return to;
}

inline TextOutput& operator<<(TextOutput& to, const bool &val)
{
    if (val) to.print("true", 4);
    else to.print("false", 5);
    return to;
}

inline TextOutput& operator<<(TextOutput& to, const String16& val)
{
    to << String8(val).c_str();
    return to;
}

// ---------------------------------------------------------------------------
// No user servicable parts below.

inline TextOutput& endl(TextOutput& to)
{
    to.print("\n", 1);
    return to;
}

inline TextOutput& indent(TextOutput& to)
{
    to.moveIndent(1);
    return to;
}

inline TextOutput& dedent(TextOutput& to)
{
    to.moveIndent(-1);
    return to;
}

inline TextOutput& operator<<(TextOutput& to, TextOutputManipFunc func)
{
    return (*func)(to);
}

inline TypeCode::TypeCode(uint32_t code) : mCode(code) { }
inline TypeCode::~TypeCode() { }
inline uint32_t TypeCode::typeCode() const { return mCode; }

inline HexDump::~HexDump() { }

inline HexDump& HexDump::setBytesPerLine(size_t bytesPerLine) {
    mBytesPerLine = bytesPerLine; return *this;
}
inline HexDump& HexDump::setSingleLineCutoff(int32_t bytes) {
    mSingleLineCutoff = bytes; return *this;
}
inline HexDump& HexDump::setAlignment(size_t alignment) {
    mAlignment = alignment; return *this;
}
inline HexDump& HexDump::setCArrayStyle(bool enabled) {
    mCArrayStyle = enabled; return *this;
}

inline const void* HexDump::buffer() const { return mBuffer; }
inline size_t HexDump::size() const { return mSize; }
inline size_t HexDump::bytesPerLine() const { return mBytesPerLine; }
inline int32_t HexDump::singleLineCutoff() const { return mSingleLineCutoff; }
inline size_t HexDump::alignment() const { return mAlignment; }
inline bool HexDump::carrayStyle() const { return mCArrayStyle; }

// ---------------------------------------------------------------------------
} // namespace android
