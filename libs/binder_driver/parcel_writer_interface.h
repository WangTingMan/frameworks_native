#pragma once

#include <array>
#include <map> // for legacy reasons
#include <string>
#include <type_traits>
#include <variant>
#include <vector>
#include <optional>
#include <memory>
#include <functional>

#include <android-base/unique_fd.h>
#include <cutils/native_handle.h>
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <utils/String16.h>
#include <utils/Vector.h>
#include <utils/Flattenable.h>

#include "linux/libbinder_driver_exports.h"

namespace android
{

class parcel_writer_interface
{

public:

    virtual ~parcel_writer_interface(){}

    virtual void copy_to( uint8_t* a_dest, uint32_t a_size ) = 0;

    virtual uint32_t size() = 0;

    virtual status_t            setDataSize( size_t size ) = 0;

public: // writing related APIs

    virtual void write_int32_t( int32_t a_value ) = 0;

    virtual status_t            write( const void* data, size_t len ) = 0;
    virtual void*               writeInplace( size_t len ) = 0;
    virtual status_t            writeUnpadded( const void* data, size_t len ) = 0;
    virtual status_t            writeInt32( int32_t val ) = 0;
    virtual status_t            writeUint32( uint32_t val ) = 0;
    virtual status_t            writeInt64( int64_t val ) = 0;
    virtual status_t            writeUint64( uint64_t val ) = 0;
    virtual status_t            writeFloat( float val ) = 0;
    virtual status_t            writeDouble( double val ) = 0;
    virtual status_t            writeCString( const char* str ) = 0;
    virtual status_t            writeString8( const String8& str ) = 0;
    virtual status_t            writeString8( const char* str, size_t len ) = 0;
    virtual status_t            writeString16( const String16& str ) = 0;
    virtual status_t            writeString16( const std::optional<String16>& str ) = 0;
    virtual status_t            writeString16( const std::unique_ptr<String16>& str ) = 0;
    virtual status_t            writeString16( const char16_t* str, size_t len ) = 0;
    virtual status_t            writeInt32Array( size_t len, const int32_t* val ) = 0;
    virtual status_t            writeByteArray( size_t len, const uint8_t* val ) = 0;
    virtual status_t            writeBool( bool val ) = 0;
    virtual status_t            writeChar( char16_t val ) = 0;
    virtual status_t            writeByte( int8_t val ) = 0;

    // Take a UTF8 encoded string, convert to UTF16, write it to the parcel.
    virtual status_t            writeUtf8AsUtf16( const std::string& str ) = 0;
    virtual status_t            writeUtf8AsUtf16( const std::optional<std::string>& str ) = 0;
    virtual status_t            writeUtf8AsUtf16( const std::unique_ptr<std::string>& str ) = 0;

    virtual status_t            writeByteVector( const std::optional<std::vector<int8_t>>& val ) = 0;
    virtual status_t            writeByteVector( const std::unique_ptr<std::vector<int8_t>>& val ) = 0;
    virtual status_t            writeByteVector( const std::vector<int8_t>& val ) = 0;
    virtual status_t            writeByteVector( const std::optional<std::vector<uint8_t>>& val ) = 0;
    virtual status_t            writeByteVector( const std::unique_ptr<std::vector<uint8_t>>& val ) = 0;
    virtual status_t            writeByteVector( const std::vector<uint8_t>& val ) = 0;
    virtual status_t            writeInt32Vector( const std::optional<std::vector<int32_t>>& val ) = 0;
    virtual status_t            writeInt32Vector( const std::unique_ptr<std::vector<int32_t>>& val ) = 0;
    virtual status_t            writeInt32Vector( const std::vector<int32_t>& val ) = 0;
    virtual status_t            writeInt64Vector( const std::optional<std::vector<int64_t>>& val ) = 0;
    virtual status_t            writeInt64Vector( const std::unique_ptr<std::vector<int64_t>>& val ) = 0;
    virtual status_t            writeInt64Vector( const std::vector<int64_t>& val ) = 0;
    virtual status_t            writeUint64Vector( const std::optional<std::vector<uint64_t>>& val ) = 0;
    virtual status_t            writeUint64Vector( const std::unique_ptr<std::vector<uint64_t>>& val ) = 0;
    virtual status_t            writeUint64Vector( const std::vector<uint64_t>& val ) = 0;
    virtual status_t            writeFloatVector( const std::optional<std::vector<float>>& val ) = 0;
    virtual status_t            writeFloatVector( const std::unique_ptr<std::vector<float>>& val ) = 0;
    virtual status_t            writeFloatVector( const std::vector<float>& val ) = 0;
    virtual status_t            writeDoubleVector( const std::optional<std::vector<double>>& val ) = 0;
    virtual status_t            writeDoubleVector( const std::unique_ptr<std::vector<double>>& val ) = 0;
    virtual status_t            writeDoubleVector( const std::vector<double>& val ) = 0;
    virtual status_t            writeBoolVector( const std::optional<std::vector<bool>>& val ) = 0;
    virtual status_t            writeBoolVector( const std::unique_ptr<std::vector<bool>>& val ) = 0;
    virtual status_t            writeBoolVector( const std::vector<bool>& val ) = 0;
    virtual status_t            writeCharVector( const std::optional<std::vector<char16_t>>& val ) = 0;
    virtual status_t            writeCharVector( const std::unique_ptr<std::vector<char16_t>>& val ) = 0;
    virtual status_t            writeCharVector( const std::vector<char16_t>& val ) = 0;
    virtual status_t            writeString16Vector(
        const std::optional<std::vector<std::optional<String16>>>& val ) = 0;
    virtual status_t            writeString16Vector(
        const std::unique_ptr<std::vector<std::unique_ptr<String16>>>& val ) = 0;
    virtual status_t            writeString16Vector( const std::vector<String16>& val ) = 0;
    virtual status_t            writeUtf8VectorAsUtf16Vector(
        const std::optional<std::vector<std::optional<std::string>>>& val ) = 0;
    virtual status_t            writeUtf8VectorAsUtf16Vector(
        const std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& val ) = 0;
    virtual status_t            writeUtf8VectorAsUtf16Vector( const std::vector<std::string>& val ) = 0;

public: // reading related APIs

    virtual void                ipcSetDataReference( const uint8_t* data, size_t dataSize, const uint64_t* objects,
                                      size_t objectsCount ) = 0;

    virtual status_t            read( void* outData, size_t len ) const = 0;
    virtual const void*         readInplace( size_t len ) const = 0;
    virtual int32_t             readInt32() const = 0;
    virtual status_t            readInt32( int32_t* pArg ) const = 0;
    virtual uint32_t            readUint32() const = 0;
    virtual status_t            readUint32( uint32_t* pArg ) const = 0;
    virtual int64_t             readInt64() const = 0;
    virtual status_t            readInt64( int64_t* pArg ) const = 0;
    virtual uint64_t            readUint64() const = 0;
    virtual status_t            readUint64( uint64_t* pArg ) const = 0;
    virtual float               readFloat() const = 0;
    virtual status_t            readFloat( float* pArg ) const = 0;
    virtual double              readDouble() const = 0;
    virtual status_t            readDouble( double* pArg ) const = 0;
    virtual bool                readBool() const = 0;
    virtual status_t            readBool( bool* pArg ) const = 0;
    virtual char16_t            readChar() const = 0;
    virtual status_t            readChar( char16_t* pArg ) const = 0;
    virtual int8_t              readByte() const = 0;
    virtual status_t            readByte( int8_t* pArg ) const = 0;

    // Read a UTF16 encoded string, convert to UTF8
    virtual status_t            readUtf8FromUtf16( std::string* str ) const = 0;
    virtual status_t            readUtf8FromUtf16( std::optional<std::string>* str ) const = 0;
    virtual status_t            readUtf8FromUtf16( std::unique_ptr<std::string>* str ) const = 0;

    virtual const char*         readCString() const = 0;
    virtual String8             readString8() const = 0;
    virtual status_t            readString8( String8* pArg ) const = 0;
    virtual const char*         readString8Inplace( size_t* outLen ) const = 0;
    virtual String16            readString16() const = 0;
    virtual status_t            readString16( String16* pArg ) const = 0;
    virtual status_t            readString16( std::optional<String16>* pArg ) const = 0;
    virtual status_t            readString16( std::unique_ptr<String16>* pArg ) const = 0;
    virtual const char16_t*     readString16Inplace( size_t* outLen ) const = 0;

    virtual status_t            finishWrite( size_t len ) = 0;
    virtual void                releaseObjects() = 0;
    virtual void                acquireObjects() = 0;
    virtual status_t            growData( size_t len ) = 0;
    // Clear the Parcel and set the capacity to `desired`.
    // Doesn't reset the RPC session association.
    virtual status_t            restartWrite( size_t desired ) = 0;
    // Set the capacity to `desired`, truncating the Parcel if necessary.
    virtual status_t            continueWrite( size_t desired ) = 0;
    virtual status_t            truncateRpcObjects( size_t newObjectsSize ) = 0;
    virtual status_t            writePointer( uintptr_t val ) = 0;
    virtual status_t            readPointer( uintptr_t* pArg ) const = 0;
    virtual uintptr_t           readPointer() const = 0;
};

using parcel_writer_maker = std::function<std::shared_ptr<parcel_writer_interface>()>;

LIBBINDERDRIVER_EXPORTS void register_parcel_writer_maker( parcel_writer_maker a_maker );

LIBBINDERDRIVER_EXPORTS void register_hidl_parcel_writer_maker( parcel_writer_maker a_maker );

LIBBINDERDRIVER_EXPORTS parcel_writer_maker get_parcel_writer_maker();

LIBBINDERDRIVER_EXPORTS parcel_writer_maker get_hidl_parcel_writer_maker();

}

