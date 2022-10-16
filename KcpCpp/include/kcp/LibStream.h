#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_STREAM_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_STREAM_H__

#pragma once

#include "kcp/common/common.h"

KCP_CPP_BEGIN

class LibStream
{
public:
    LibStream();
    virtual ~LibStream();
    LibStream(LibStream &&other);

    // 缓存初始化
    void InitBuffer(UInt64 bufferSize);
    void AttachBuffer(Byte8 *buffer, UInt64 bytes, UInt64 readPos = 0, UInt64 writePos = 0);
    Byte8 *PopBuffer();
    void Reset();
    void Destroy();
    void AppendCapacity(UInt64 appendSize);
    Byte8 *GetBuffer();
    const Byte8 *GetBuffer() const;
    UInt64 GetCapicity() const;
    bool IsAttach() const;
    void Compress();
    UInt64 GetReadPos() const;
    UInt64 GetWritePos() const;
    void Swap(LibStream &stream);
    void Takeover(LibStream &other);
    void Giveup();

    #pragma region // 读
    UInt64 GetReadBytes() const;
    UInt64 GetReadableBytes() const;
    Byte8 *GetReadBegin();
    const Byte8 *GetReadBegin() const;

    // return(UInt64): 返回真实位移字节数
    UInt64 ShiftReadPos(UInt64 offsetSize);

    Byte8 ReadInt8();
    Int16 ReadInt16();
    Int32 ReadInt32();
    Int64 ReadInt64();
    U8    ReadUInt8();
    UInt16 ReadUInt16();
    UInt32 ReadUInt32();
    UInt64 ReadUInt64();
    Float ReadFloat();
    Double ReadDouble();
    UInt64 Read(void *dest, UInt64 readBytes);   // readBytes:输入要读的字节数,return:真实读取的数据大小
    #pragma endregion

    #pragma region // 写
    UInt64 GetWrittenBytes() const;
    UInt64 GetWritableBytes() const;
    Byte8 *GetWriteBegin();

    // return(UInt64): 返回真实位移的字节数
    UInt64 ShiftWritePos(UInt64 offsetSize);

    bool WriteInt8(Byte8 n);
    bool WriteInt16(Int16 n);
    bool WriteInt32(Int32 n);
    bool WriteFloat(Float n);
    bool WriteDouble(Double n);
    bool WriteUInt64(UInt64 n);
    bool WriteInt64(Int64 n);
    bool Write(const void *data, UInt64 dataSize);
    #pragma endregion

private:
    // attach模式下不可扩展缓存
    bool _isAttach = false;

    Byte8 *_buffer = NULL;
    UInt64 _capicity = 0;
    UInt64 _readPos = 0;
    UInt64 _writePos = 0;
};

inline LibStream::LibStream()
:_isAttach(false)
,_buffer(NULL)
,_capicity(0)
,_readPos(0)
,_writePos(0)
{
}

inline LibStream::~LibStream()
{
    Destroy();
}

inline LibStream::LibStream(LibStream &&other)
{
    Swap(other);
}

inline void LibStream::InitBuffer(UInt64 bufferSize)
{
    Destroy();    

    _capicity = bufferSize;
    _buffer = new Byte8[bufferSize];
    _isAttach = false;
    _readPos = 0;
    _writePos = 0;
}

inline void LibStream::AttachBuffer(Byte8 *buffer, UInt64 bytes, UInt64 readPos, UInt64 writePos)
{
    Destroy();

    _isAttach = true;
    _buffer = buffer;
    _capicity = bytes;
    _readPos = readPos;
    _writePos = writePos;
}

inline Byte8 *LibStream::PopBuffer()
{
    auto buffer = _buffer;

    _isAttach = false;
    _buffer = NULL;
    _capicity = 0;
    _readPos = 0;
    _writePos = 0;
    
    return buffer;
}

inline void LibStream::Reset()
{
    _readPos = 0;
    _writePos = 0;
}

inline void LibStream::Destroy()
{
    if(!_isAttach && _buffer)
        delete [] _buffer;
    
    _buffer = NULL;
    _isAttach = false;
    _capicity = 0;
    _readPos = 0;
    _writePos = 0;
}

inline void LibStream::AppendCapacity(UInt64 appendSize)
{
    _capicity += appendSize;
    if(_buffer)
        _buffer = LibCast<Byte8>(::realloc(_buffer, _capicity));
    else
        _buffer = LibCast<Byte8>(::malloc(_capicity));
}

inline Byte8 *LibStream::GetBuffer()
{
    return _buffer;
}

inline const Byte8 *LibStream::GetBuffer() const
{
    return _buffer;
}

inline UInt64 LibStream::GetCapicity() const
{
    return _capicity;
}

inline bool LibStream::IsAttach() const
{
    return _isAttach;
}

inline void LibStream::Compress()
{
    if(!_buffer)
        return;

    // 移动数据
    if(_readPos != 0)
        ::memcpy(_buffer, _buffer + _readPos, _writePos - _readPos);

    _writePos -= _readPos;
    _readPos = 0;

    // 托管的不可改变内存大小
    if(_isAttach)
        return;

    _capicity = _writePos;
    _buffer = LibCast<Byte8>(::realloc(_buffer, _capicity));
}

inline UInt64 LibStream::GetReadPos() const
{
    return _readPos;
}

inline UInt64 LibStream::GetWritePos() const
{
    return _writePos;
}

inline void LibStream::Swap(LibStream &other)
{
    auto otherAttach = other._isAttach;
    auto otherBuffer = other._buffer;
    auto otherCapicity = other._capicity;
    auto otherReadPos = other._readPos;
    auto otherWritePos = other._writePos;
    other._isAttach = _isAttach;
    other._buffer = _buffer;
    other._capicity = _capicity;
    other._readPos = _readPos;
    other._writePos = _writePos;

    _isAttach = otherAttach;
    _buffer = otherBuffer;
    _capicity = otherCapicity;
    _readPos = otherReadPos;
    _writePos = otherWritePos;
}

inline void LibStream::Takeover(LibStream &other)
{
    // 销毁自身
    Destroy();

    // 转移
    _isAttach = other._isAttach;
    _buffer = other._buffer;
    _capicity = other._capicity;
    _readPos = other._readPos;
    _writePos = other._writePos;

    // 放弃
    other.Giveup();
}

inline void LibStream::Giveup()
{

}


inline UInt64 LibStream::GetReadBytes() const
{
    return _readPos;   
}

inline UInt64 LibStream::GetReadableBytes() const
{
    return (_writePos > _readPos) ? (_writePos - _readPos) : 0;
}

inline Byte8 *LibStream::GetReadBegin()
{
    return _buffer + _readPos;
}

inline const Byte8 *LibStream::GetReadBegin() const
{
    return _buffer + _readPos;
}

inline UInt64 LibStream::ShiftReadPos(UInt64 offsetSize)
{
    auto readableSize = GetReadableBytes();
    offsetSize = (offsetSize > readableSize) ? readableSize : offsetSize;
    _readPos += offsetSize;

    return offsetSize;
}

inline Byte8 LibStream::ReadInt8()
{
    Byte8 data = 0;
    Read(&data, 1);
    return data;
}

inline Int16 LibStream::ReadInt16()
{
    Int16 data = 0;
    Read(&data, 2);
    return data;
}

inline Int32 LibStream::ReadInt32()
{
    Int32 data = 0;
    Read(&data, 4);
    return data;
}

inline Int64 LibStream::ReadInt64()
{
    Int64 data = 0;
    Read(&data, 8);
    return data;
}

inline U8 LibStream::ReadUInt8()
{
    U8 data = 0;
    Read(&data, 1);
    return data;
}

inline UInt16 LibStream::ReadUInt16()
{
    UInt16 data = 0;
    Read(&data, 2);
    return data;
}

inline UInt32 LibStream::ReadUInt32()
{
    UInt32 data = 0;
    Read(&data, 4);
    return data;
}

inline UInt64 LibStream::ReadUInt64()
{
    UInt64 data = 0;
    Read(&data, 8);
    return data;
}

inline Float LibStream::ReadFloat()
{
    Float data = 0;
    Read(&data, sizeof(Float));
    return data;
}

inline Double LibStream::ReadDouble()
{
    Double data = 0;
    Read(&data, sizeof(Double));
    return data;
}

inline UInt64 LibStream::Read(void *dest, UInt64 readBytes)
{
    UInt64 readSize = GetReadableBytes();
    readSize = (readSize > readBytes) ? readBytes : readSize;
    if(readSize == 0)
        return 0;

    ::memcpy(dest, GetReadBegin(), readSize);
    _readPos += readSize;

    return readSize;
}


inline UInt64 LibStream::GetWrittenBytes() const
{
    return _writePos;
}

inline UInt64 LibStream::GetWritableBytes() const
{
    return (_capicity > _writePos) ? (_capicity - _writePos) : 0;
}

inline Byte8 *LibStream::GetWriteBegin()
{
    return _buffer + _writePos;
}

inline UInt64 LibStream::ShiftWritePos(UInt64 offsetSize)
{
    auto finalOffset = GetWritableBytes();
    finalOffset = (finalOffset > offsetSize) ? offsetSize : finalOffset;
    _writePos += finalOffset;

    return finalOffset;
}

inline bool LibStream::WriteInt8(Byte8 n)
{
    return Write(&n, 1);
}

inline bool LibStream::WriteInt16(Int16 n)
{
    return Write(&n, 2);
}

inline bool LibStream::WriteInt32(Int32 n)
{
    return Write(&n, 4);
}

inline bool LibStream::WriteFloat(Float n)
{
    return Write(&n, sizeof(n));
}

inline bool LibStream::WriteDouble(Double n)
{
    return Write(&n, sizeof(n));
}

inline bool LibStream::WriteUInt64(UInt64 n)
{
    return Write(&n, 8);
}

inline bool LibStream::WriteInt64(Int64 n)
{
    return Write(&n, 8);
}

inline bool LibStream::Write(const void *data, UInt64 dataSize)
{
    if(dataSize > GetWritableBytes())
    {
        // 写满又是托管的没有权限扩展缓存
        if(_isAttach)
            return false;

        // 2倍
        _capicity += std::max<UInt64>(dataSize, _capicity);
        _buffer = LibCast<Byte8>(::realloc(_buffer, _capicity));
    }

    ::memcpy(GetWriteBegin(), data, dataSize);
    _writePos += dataSize;

    return true;
}
    
KCP_CPP_END

#endif
