
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOG_DATA_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOG_DATA_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>
#include <kcp/LibTime.h>

KCP_CPP_BEGIN

struct LogData
{
    LibTime _logTime;
    LibString _logInfo;

    UInt64 CalcBytes() const;
};

inline UInt64 LogData::CalcBytes() const
{
    return __MEMORY_ALIGN__( sizeof(LogData) + _logInfo.GetRaw().size());
}

KCP_CPP_END

#endif
