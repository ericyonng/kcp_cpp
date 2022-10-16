
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_FILE_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_FILE_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibTime.h>
#include <kcp/LibString.h>
#include <kcp/Locker.h>

KCP_CPP_BEGIN

struct InitFileInfo
{
    LibString _fileWholeName;
    bool _isCreate;
    LibString _openMode;
    bool _useTimestampTailer;
    LibTime _nowTime;
    
};

class LibFile
{

public:
    LibFile();
    virtual ~LibFile();

public:
    virtual bool Init(const Byte8 *fileWholeName
        , bool isCreate = false
        , const Byte8 *openMode = "ab+"
        , bool useTimestampTailer = false
        , LibTime *nowTime = NULL);

    virtual bool Open(const Byte8 *fileWholeName
    , bool *IsFileExist = NULL
    , bool isCreate = false
    , const Byte8 *openMode = "ab+"
    , bool useTimestampTailer = false
    , LibTime *nowTime = NULL);

    virtual bool Reopen(LibTime *nowTime = NULL);
    virtual bool Flush();
    virtual bool Close();

    virtual Int64 Write(const void *buffer, UInt64 writeDataLen);
    virtual Int64 Read(LibString &outBuffer);
    virtual Int64 Read(Int64 sizeLimit, LibString &outBuffer);
    virtual Int64 ReadOneLine(LibString &outBuffer);

    void Lock();
    void UnLock();

    bool IsOpen() const;
    const LibString &GetPath() const;
    const LibString &GetFileName() const;
    Int64 GetSize() const;
    void GetCurrentFileName(LibString &curName) const;
    FILE *GetFp();

public:
    operator bool() const;
    operator FILE *();
    operator const FILE *() const;

protected:
    void _BuildFileName(const Byte8 *fileName
    , bool useTimeTail
    , LibString &fileNameOut
    , LibString &extensionNameOut) const;

    void _BuildFileName(LibString &fileNameOut) const;

protected:
    FILE *_fp = NULL;
    Int64 _fileSize;
    LibTime _createFileTime;
    LibTime _modifyFileTime;
    bool _useTimestampTailer = false;
    LibString _path;
    LibString _fileName;
    LibString _extensionName;       // 扩展名
    LibString _openMode = "ab+";
    Locker  _locker;
};


inline void LibFile::Lock()
{
    _locker.Lock();
}

inline void LibFile::UnLock()
{
    _locker.Unlock();
}

inline bool LibFile::IsOpen() const
{
    return _fp != NULL;
}

inline const LibString &LibFile::GetPath() const
{
    return _path;
}

inline const LibString &LibFile::GetFileName() const
{
    return _fileName;
}

inline Int64 LibFile::GetSize() const
{
    return _fileSize;
}

inline void LibFile::GetCurrentFileName(LibString &curName) const
{
    _BuildFileName(curName);
}

inline FILE *LibFile::GetFp()
{
    return _fp;
}

inline LibFile::operator bool() const
{
    return _fp != NULL;
}

inline LibFile::operator FILE *()
{
    return _fp;
}

inline LibFile::operator const FILE *() const
{
    return _fp;
}

KCP_CPP_END

#endif
