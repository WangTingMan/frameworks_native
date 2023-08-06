#pragma once

#include <binder/Parcel.h>
#include <parcel_writer_interface.h>

namespace android
{

void register_detail_parcel_writer();

class parcel_writer_impl : public parcel_writer_interface
{

public:

    parcel_writer_impl(){}

    void copy_to( uint8_t* a_dest, uint32_t a_size );

    uint32_t size();

    status_t            setDataSize( size_t a_size )
    {
        return m_detail.setDataSize( a_size );
    }

    void write_int32_t( int32_t a_value )
    {
        m_detail.writeInt32( a_value );
    }

    status_t            write( const void* data, size_t len )
    {
        return m_detail.write( data, len );
    }

    void* writeInplace( size_t len )
    {
        return m_detail.writeInplace( len );
    }

    status_t            writeUnpadded( const void* data, size_t len )
    {
        return m_detail.writeUnpadded( data, len );
    }

    status_t            writeInt32( int32_t val )
    {
        return m_detail.writeInt32( val );
    }

    status_t            writeUint32( uint32_t val )
    {
        return m_detail.writeUint32( val );
    }

    status_t            writeInt64( int64_t val )
    {
        return m_detail.writeInt64( val );
    }

    status_t            writeUint64( uint64_t val )
    {
        return m_detail.writeUint64( val );
    }

    status_t            writeFloat( float val )
    {
        return m_detail.writeFloat( val );
    }

    status_t            writeDouble( double val )
    {
        return m_detail.writeDouble( val );
    }

    status_t            writeCString( const char* str )
    {
        return m_detail.writeCString( str );
    }

    status_t            writeString8( const String8& str )
    {
        return m_detail.writeString8( str );
    }

    status_t            writeString8( const char* str, size_t len )
    {
        return m_detail.writeString8( str, len );
    }

    status_t            writeString16( const String16& str )
    {
        return m_detail.writeString16( str );
    }

    status_t            writeString16( const std::optional<String16>& str )
    {
        return m_detail.writeString16( str );
    }

    status_t            writeString16( const std::unique_ptr<String16>& str )
    {
        return m_detail.writeString16( str );
    }

    status_t            writeString16( const char16_t* str, size_t len )
    {
        return m_detail.writeString16( str, len );
    }

    status_t            writeInt32Array( size_t len, const int32_t* val )
    {
        return m_detail.writeInt32Array( len, val );
    }

    status_t            writeByteArray( size_t len, const uint8_t* val )
    {
        return m_detail.writeByteArray( len, val );
    }

    status_t            writeBool( bool val )
    {
        return m_detail.writeBool( val );
    }

    status_t            writeChar( char16_t val )
    {
        return m_detail.writeChar( val );
    }

    status_t            writeByte( int8_t val )
    {
        return m_detail.writeByte( val );
    }


    // Take a UTF8 encoded string, convert to UTF16, write it to the parcel.
    status_t            writeUtf8AsUtf16( const std::string& str )
    {
        return m_detail.writeUtf8AsUtf16( str );
    }

    status_t            writeUtf8AsUtf16( const std::optional<std::string>& str )
    {
        return m_detail.writeUtf8AsUtf16( str );
    }

    status_t            writeUtf8AsUtf16( const std::unique_ptr<std::string>& str )
    {
        return m_detail.writeUtf8AsUtf16( str );
    }


    status_t            writeByteVector( const std::optional<std::vector<int8_t>>& val )
    {
        return m_detail.writeByteVector( val );
    }

    status_t            writeByteVector( const std::unique_ptr<std::vector<int8_t>>& val )
    {
        return m_detail.writeByteVector( val );
    }

    status_t            writeByteVector( const std::vector<int8_t>& val )
    {
        return m_detail.writeByteVector( val );
    }

    status_t            writeByteVector( const std::optional<std::vector<uint8_t>>& val )
    {
        return m_detail.writeByteVector( val );
    }

    status_t            writeByteVector( const std::unique_ptr<std::vector<uint8_t>>& val )
    {
        return m_detail.writeByteVector( val );
    }

    status_t            writeByteVector( const std::vector<uint8_t>& val )
    {
        return m_detail.writeByteVector( val );
    }

    status_t            writeInt32Vector( const std::optional<std::vector<int32_t>>& val )
    {
        return m_detail.writeInt32Vector( val );
    }

    status_t            writeInt32Vector( const std::unique_ptr<std::vector<int32_t>>& val )
    {
        return m_detail.writeInt32Vector( val );
    }

    status_t            writeInt32Vector( const std::vector<int32_t>& val )
    {
        return m_detail.writeInt32Vector( val );
    }

    status_t            writeInt64Vector( const std::optional<std::vector<int64_t>>& val )
    {
        return m_detail.writeInt64Vector( val );
    }

    status_t            writeInt64Vector( const std::unique_ptr<std::vector<int64_t>>& val )
    {
        return m_detail.writeInt64Vector( val );
    }

    status_t            writeInt64Vector( const std::vector<int64_t>& val )
    {
        return m_detail.writeInt64Vector( val );
    }

    status_t            writeUint64Vector( const std::optional<std::vector<uint64_t>>& val )
    {
        return m_detail.writeUint64Vector( val );
    }

    status_t            writeUint64Vector( const std::unique_ptr<std::vector<uint64_t>>& val )
    {
        return m_detail.writeUint64Vector( val );
    }
    status_t            writeUint64Vector( const std::vector<uint64_t>& val )
    {
        return m_detail.writeUint64Vector( val );
    }
    status_t            writeFloatVector( const std::optional<std::vector<float>>& val )
    {
        return m_detail.writeFloatVector( val );
    }
    status_t            writeFloatVector( const std::unique_ptr<std::vector<float>>& val )
    {
        return m_detail.writeFloatVector( val );
    }
    status_t            writeFloatVector( const std::vector<float>& val )
    {
        return m_detail.writeFloatVector( val );
    }
    status_t            writeDoubleVector( const std::optional<std::vector<double>>& val )
    {
        return m_detail.writeDoubleVector( val );
    }
    status_t            writeDoubleVector( const std::unique_ptr<std::vector<double>>& val )
    {
        return m_detail.writeDoubleVector( val );
    }
    status_t            writeDoubleVector( const std::vector<double>& val )
    {
        return m_detail.writeDoubleVector( val );
    }
    status_t            writeBoolVector( const std::optional<std::vector<bool>>& val )
    {
        return m_detail.writeBoolVector( val );
    }
    status_t            writeBoolVector( const std::unique_ptr<std::vector<bool>>& val )
    {
        return m_detail.writeBoolVector( val );
    }
    status_t            writeBoolVector( const std::vector<bool>& val )
    {
        return m_detail.writeBoolVector( val );
    }
    status_t            writeCharVector( const std::optional<std::vector<char16_t>>& val )
    {
        return m_detail.writeCharVector( val );
    }
    status_t            writeCharVector( const std::unique_ptr<std::vector<char16_t>>& val )
    {
        return m_detail.writeCharVector( val );
    }
    status_t            writeCharVector( const std::vector<char16_t>& val )
    {
        return m_detail.writeCharVector( val );
    }
    status_t            writeString16Vector(
        const std::optional<std::vector<std::optional<String16>>>& val )
    {
        return m_detail.writeString16Vector( val );
    }
    status_t            writeString16Vector(
        const std::unique_ptr<std::vector<std::unique_ptr<String16>>>& val )
    {
        return m_detail.writeString16Vector( val );
    }
    status_t            writeString16Vector( const std::vector<String16>& val )
    {
        return m_detail.writeString16Vector( val );
    }
    status_t            writeUtf8VectorAsUtf16Vector(
        const std::optional<std::vector<std::optional<std::string>>>& val )
    {
        return m_detail.writeUtf8VectorAsUtf16Vector( val );
    }
    status_t            writeUtf8VectorAsUtf16Vector(
        const std::unique_ptr<std::vector<std::unique_ptr<std::string>>>& val )
    {
        return m_detail.writeUtf8VectorAsUtf16Vector( val );
    }
    status_t            writeUtf8VectorAsUtf16Vector( const std::vector<std::string>& val )
    {
        return m_detail.writeUtf8VectorAsUtf16Vector( val );
    }

public: // reading APIs

    static void release_func_taker( const uint8_t* data, size_t dataSize, const binder_size_t* objects,
                                    size_t objectsSize )
    {

    }

     void ipcSetDataReference( const uint8_t* data, size_t dataSize, const binder_size_t* objects,
                              size_t objectsCount )
     {
          m_detail.ipcSetDataReference( data, dataSize, objects, objectsCount, &release_func_taker );
     }

     status_t            read( void* outData, size_t len ) const
     {
         return m_detail.read(outData, len);
     }

     const void*         readInplace( size_t len ) const
     {
         return m_detail.readInplace(len);
     }

     int32_t             readInt32() const
     {
         return m_detail.readInt32();
     }

     status_t            readInt32( int32_t* pArg ) const
     {
         return m_detail.readInt32(pArg);
     }

     uint32_t            readUint32() const
     {
         return m_detail.readUint32();
     }

     status_t            readUint32( uint32_t* pArg ) const
     {
         return m_detail.readUint32(pArg);
     }

     int64_t             readInt64() const
     {
         return m_detail.readInt64();
     }

     status_t            readInt64( int64_t* pArg ) const
     {
         return m_detail.readInt64(pArg);
     }

     uint64_t            readUint64() const
     {
         return m_detail.readUint64();
     }

     status_t            readUint64( uint64_t* pArg ) const
     {
         return m_detail.readUint64(pArg);
     }

     float               readFloat() const
     {
         return m_detail.readFloat();
     }

     status_t            readFloat( float* pArg ) const
     {
         return m_detail.readFloat(pArg);
     }

     double              readDouble() const
     {
         return m_detail.readDouble();
     }

     status_t            readDouble( double* pArg ) const
     {
         return m_detail.readDouble(pArg);
     }

     bool                readBool() const
     {
         return m_detail.readBool();
     }

     status_t            readBool( bool* pArg ) const
     {
         return m_detail.readBool(pArg);
     }

     char16_t            readChar() const
     {
         return m_detail.readChar();
     }

     status_t            readChar( char16_t* pArg ) const
     {
         return m_detail.readChar(pArg);
     }

     int8_t              readByte() const
     {
         return m_detail.readByte();
     }

     status_t            readByte( int8_t* pArg ) const
     {
         return m_detail.readByte(pArg);
     }


    // Read a UTF16 encoded string, convert to UTF8
     status_t            readUtf8FromUtf16( std::string* str ) const
     {
         return m_detail.readUtf8FromUtf16( str );
     }

     status_t            readUtf8FromUtf16( std::optional<std::string>* str ) const
     {
         return m_detail.readUtf8FromUtf16(str);
     }

     status_t            readUtf8FromUtf16( std::unique_ptr<std::string>* str ) const
     {
         return m_detail.readUtf8FromUtf16(str);
     }


     const char* readCString() const
     {
         return m_detail.readCString();
     }

     String8             readString8() const
     {
         return m_detail.readString8();
     }

     status_t            readString8( String8* pArg ) const
     {
         return m_detail.readString8(pArg);
     }

     const char* readString8Inplace( size_t* outLen ) const
     {
         return m_detail.readString8Inplace(outLen);
     }

     String16            readString16() const
     {
         return m_detail.readString16();
     }

     status_t            readString16( String16* pArg ) const
     {
         return m_detail.readString16(pArg);
     }

     status_t            readString16( std::optional<String16>* pArg ) const
     {
         return m_detail.readString16(pArg);
     }

     status_t            readString16( std::unique_ptr<String16>* pArg ) const
     {
         return m_detail.readString16(pArg);
     }

     const char16_t* readString16Inplace( size_t* outLen ) const
     {
         return m_detail.readString16Inplace(outLen);
     }


     status_t            finishWrite( size_t len )
     {
         return m_detail.finishWrite(len);
     }

     void                releaseObjects()
     {
         return m_detail.releaseObjects();
     }

     void                acquireObjects()
     {
         return m_detail.acquireObjects();
     }

     status_t            growData( size_t len )
     {
         return m_detail.growData(len);
     }

    // Clear the Parcel and set the capacity to `desired`.
    // Doesn't reset the RPC session association.
     status_t            restartWrite( size_t desired )
     {
         return m_detail.restartWrite(desired);
     }

    // Set the capacity to `desired`, truncating the Parcel if necessary.
     status_t            continueWrite( size_t desired )
     {
         return m_detail.continueWrite(desired);
     }

     status_t            truncateRpcObjects( size_t newObjectsSize )
     {
         return m_detail.truncateRpcObjects(newObjectsSize);
     }

     status_t            writePointer( uintptr_t val )
     {
         return m_detail.writePointer(val);
     }

     status_t            readPointer( uintptr_t* pArg ) const
     {
         return m_detail.readPointer(pArg);
     }

     uintptr_t           readPointer() const
     {
         return m_detail.readPointer();
     }


private:

    Parcel m_detail;
};


}


