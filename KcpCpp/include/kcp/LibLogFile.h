

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_LOG_FILE_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_LOG_FILE_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibFile.h>

KCP_CPP_BEGIN

class LibLogFile : public LibFile
{
public:
    LibLogFile();
    virtual ~LibLogFile();

public:
    bool IsDayPass(const LibTime &nowTime) const;
    void UpdateLastPassDayTime(LibTime *nowTime = NULL);

    // 分割文件
    bool IsTooLarge(Int64 limitSize) const;

    // isSysFirstCreate=true:系统第一次创建不会分文件
    void PartitionFile(bool isSysFirstCreate = false, LibTime *nowTime = NULL);    

protected:
    Int32 _partNo;
    LibTime  _lastPassDayTime;
};

inline bool LibLogFile::IsTooLarge(Int64 limitSize) const
{
    if(limitSize <= 0)
        return false;

    return _fileSize >= limitSize;
}

inline bool LibLogFile::IsDayPass(const LibTime &nowTime) const
{
    return _lastPassDayTime.GetZeroTime() != nowTime.GetZeroTime();
}

inline void LibLogFile::UpdateLastPassDayTime(LibTime *nowTime)
{
    if(!nowTime)
    {
        _lastPassDayTime.UpdateTime();
        return;
    }

    _lastPassDayTime = *nowTime;
}

KCP_CPP_END

#endif
