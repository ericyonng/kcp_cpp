
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_TIMER_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_TIMER_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibDelegate.h>
#include <kcp/TimeData.h>
#include <kcp/util/TimeUtil.h>

KCP_CPP_BEGIN

class TimerMgr;

class LibTimer
{
public:
    LibTimer(TimerMgr *mgr);
    virtual ~LibTimer();

public:
    void Cancel();
    void Schedule(Int64 milliSec);
    void Schedule(Int64 startTime, Int64 milliSecPeriod);
    bool IsScheduling() const;

    // 设置超时回调
    template<typename ObjType>
    void SetTimeOutHandler(ObjType *obj, void (ObjType::*handler)(LibTimer *));
    void SetTimeOutHandler(void (*handler)(LibTimer *));
    void SetTimeOutHandler(IDelegate<void, LibTimer *> *delg);
    // 设置cancel回调
    template<typename ObjType>
    void SetCancelHandler(ObjType *obj, void (ObjType::*handler)(LibTimer *));
    void SetCancelHandler(void (*handler)(LibTimer *));
    void SetCancelHandler(IDelegate<void, LibTimer *> *delg);
    // mgr调用
    void OnTimeOut();

    LibString ToString() const;
    TimerMgr *GetMgr();

    const TimeData *GetData() const;

private:
    TimerMgr *_mgr;
    TimeData *_data;
    IDelegate<void, LibTimer *> *_timeroutHandler;
    IDelegate<void, LibTimer *> *_cancelHandler;
};

inline void LibTimer::Schedule(Int64 milliSec)
{
    Schedule(TimeUtil::GetMicroTimestamp(), milliSec);
}

inline bool LibTimer::IsScheduling() const
{
    return _data->_isScheduing;
}

template<typename ObjType>
inline void LibTimer::SetTimeOutHandler(ObjType *obj, void (ObjType::*handler)(LibTimer *))
{
    if(_timeroutHandler)
        delete _timeroutHandler;
    _timeroutHandler = DelegateFactory::Create(obj, handler);
}

inline void LibTimer::SetTimeOutHandler(void (*handler)(LibTimer *))
{
    if (_timeroutHandler)
        delete _timeroutHandler;
    _timeroutHandler = DelegateFactory::Create(handler);
}

inline void LibTimer::SetTimeOutHandler(IDelegate<void, LibTimer *> *delg)
{
    if (_timeroutHandler)
        delete _timeroutHandler;
    _timeroutHandler = delg;
}

// 设置cancel回调
template<typename ObjType>
inline void LibTimer::SetCancelHandler(ObjType *obj, void (ObjType::*handler)(LibTimer *))
{
    if(_cancelHandler)
        delete _cancelHandler;
    _cancelHandler = DelegateFactory::Create(obj, handler);
}

inline void LibTimer::SetCancelHandler(void (*handler)(LibTimer *))
{
    if (_cancelHandler)
        delete _cancelHandler;
    _cancelHandler = DelegateFactory::Create(handler);
}

inline void LibTimer::SetCancelHandler(IDelegate<void, LibTimer *> *delg)
{
    if (_cancelHandler)
        delete _cancelHandler;
    _cancelHandler = delg;
}

inline void LibTimer::OnTimeOut()
{
    _timeroutHandler->Invoke(this);
}

inline TimerMgr *LibTimer::GetMgr()
{
    return _mgr;
}

inline const TimeData *LibTimer::GetData() const
{
    return _data;
}

KCP_CPP_END

#endif