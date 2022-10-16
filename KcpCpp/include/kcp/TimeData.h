
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIME_DATA_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIME_DATA_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN

class TimeData;

class TimeDataComp
{
public:
    bool operator()(const TimeData *l, const TimeData *r) const;
};

class LibTimer;

class AsynTimeData;

class TimeData
{

public:
    TimeData(Int64 id, LibTimer *timer);
    ~TimeData();
    void Release();

    LibString ToString() const;

    const Int64 _id;               // 定时器id
    Int64 _expiredTime;             // 当前过期时间 微妙
    Int64 _period;                  // 定时周期 微妙
    LibTimer *_owner;               // 定时器对象

    bool _isScheduing;              // 处于定时阶段
    AsynTimeData *_asynData;        // 异步数据 这个数据由TimerMgr统一创建与释放，这里只引用
};

inline bool TimeDataComp::operator()(const TimeData *l, const TimeData *r) const
{
    if(!l || !r)
        return l < r;

    if(l == r)
        return false;
    
    if(l->_expiredTime == r->_expiredTime)
        return l->_id < r->_id;

    return l->_expiredTime < r->_expiredTime;
}

inline void TimeData::Release()
{
    delete this;
}


KCP_CPP_END

#endif
