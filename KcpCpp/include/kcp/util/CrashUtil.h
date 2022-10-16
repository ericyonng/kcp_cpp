#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_CRASH_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_CRASH_UTIL_H__

#pragma once

#include "kcp/common/common.h"
#include "kcp/LibString.h"
#include "kcp/LibDelegate.h"

KCP_CPP_BEGIN

class ILog;
struct LogData;

class CrashUtil
{
public:
    static Int32 InitCrashHandleParams(ILog *log, IDelegate<void> *crashDestroy);

    static LibString CrystalCaptureStackBackTrace(size_t skipFrames = 0, size_t captureFrames = INFINITE);

protected:
    static void _OnBeforeCrashLogHook(LogData *logData);

    static Int32 _InitSymbol();
};

KCP_CPP_END

#endif

