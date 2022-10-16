#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIMER_DEFS_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIMER_DEFS_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibDelegate.h>

KCP_CPP_BEGIN

class LibTimer;

struct BuildTimerInfo
{
    IDelegate<void, LibTimer *> *_timeoutDelg = NULL;                   // 超时回调
    IDelegate<void, LibTimer *, Int32> *_completeCallback = NULL;       // 执行完成回调, 新的timer, status
    Int64 _startTime = 0;                                               // 开始定时时间
    Int64 _newPeriod = 0;                                               // 定时周期
};

KCP_CPP_END

#endif
