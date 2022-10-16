

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIMER_MGR_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIMER_MGR_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/TimeData.h>
#include <kcp/AsynTimeData.h>
#include <kcp/util/SystemUtil.h>
#include <kcp/util/TimeUtil.h>
#include <kcp/LibDelegate.h>

KCP_CPP_BEGIN

struct BuildTimerInfo;

// 不支持多线程,请在单线程使用定时器
class TimerMgr
{
    
public:
    TimerMgr();
    virtual ~TimerMgr();

public:
    // 在线程中启动
    void Launch(IDelegate<void> *wakeupThreadCb);
    void Drive();

    // 添加定时器 线程安全,其他接口线程不安全 ,统一线程不建议使用AddTimer 因为params会进行拷贝
    // @param()
    void AddTimer(IDelegate<void, LibTimer *> *timeOutCallback
                , Int64 startTime
                , Int64 newPeriod
                , IDelegate<void, LibTimer *, Int32> *completeCallback = NULL);
    void Schedule(IDelegate<void, LibTimer *> *timeOutCallback
            , Int64 newPeriod
            , IDelegate<void, LibTimer *, Int32> *completeCallback = NULL);
    // 注册
    void Register(TimeData *timeData, Int64 newExpireTime, Int64 newPeriod);
    // 反注册
    void UnRegister(TimeData *timeData);
    // 微妙
    Int64 GetCurFrameTime() const;
    // 新建定时数据 生成与释放由TimerMgr管理
    TimeData *NewTimeData(LibTimer *timer);
    // 定时器销毁
    void OnTimerDestroy(TimeData *timeData);
    // 微妙
    const Int64 GetNowMicroSecond() const;
    // 获取即将处理的时间间隔时间 单位ms -1 表示没有
    Int64 GetTimeoutIntervalRecently(Int64 nowMs) const;

    // 负载情况
    UInt64 GetTimerLoaded() const;

private:
    void _BeforeDrive();
    void _AfterDrive();

    void _AsynRegister(TimeData *timeData, Int64 newPeriod, Int64 newExpiredTime);
    void _Register(TimeData *timeData, Int64 newPeriod, Int64 newExpiredTime);
    void _AsynUnRegister(TimeData *timeData);
    void _UnRegister(TimeData *timeData);
    void _Destroy(TimeData *timeData);
    void _AsynDestroy(TimeData *timeData);
    void _DoPendings();

    bool _IsInTimerThread();
    void _WakeupMgrThread();

private:
    UInt64 _launchThreadId;         // 启用TimerMgr的线程
    Int64 _driving;                 // 正在驱动
    Int64 _curTime;                 // 当前时间戳 微妙

    // 过期定时队列
    std::set<TimeData *, TimeDataComp> _expireQueue;
    Int64 _curMaxId;                // 定时id

    std::set<TimeData *> _allTimeData;
    std::set<AsynTimeData *> _asynDirty;

    // 唤醒线程回调
    IDelegate<void> *_wakeupCb;

    // 双链表结构可以使用swap
    SpinLock _lck;
    std::list<BuildTimerInfo *> *_pending;
    std::list<BuildTimerInfo *> *_pendingCache;
};

inline void TimerMgr::Schedule(IDelegate<void, LibTimer *> *timeOutCallback
, Int64 newPeriod
, IDelegate<void, LibTimer *, Int32> *completeCallback)
{
    auto now = TimeUtil::GetMicroTimestamp();
    AddTimer(timeOutCallback, now, newPeriod, completeCallback);
}

inline void TimerMgr::Register(TimeData *timeData, Int64 newExpireTime, Int64 newPeriod)
{
    if (_driving > 0)
    {
        _AsynRegister(timeData, newPeriod, newExpireTime);
        return;
    }

    _Register(timeData, newPeriod, newExpireTime);
}

inline void TimerMgr::UnRegister(TimeData *timeData)
{
    if(_driving > 0)
    {
        _AsynUnRegister(timeData);
        return;
    }

    _UnRegister(timeData);
}

inline Int64 TimerMgr::GetCurFrameTime() const
{
    return _curTime;
}

inline TimeData *TimerMgr::NewTimeData(LibTimer *timer)
{
    auto newData = new TimeData(++_curMaxId, timer);
    _allTimeData.insert(newData);
    return newData;
}

inline void TimerMgr::OnTimerDestroy(TimeData *timeData)
{
    if(_driving > 0)
    {
        _AsynDestroy(timeData);
        return;
    }

    _Destroy(timeData);
}

inline Int64 TimerMgr::GetTimeoutIntervalRecently(Int64 nowMs) const
{
    if(UNLIKELY(_allTimeData.empty()))
        return -1;

    TimeData *recently = *_allTimeData.begin();
    Int64 diff = recently->_expiredTime - nowMs;
    return diff > 0 ? diff : 0;    
}

inline UInt64 TimerMgr::GetTimerLoaded() const
{
    return _allTimeData.size();
}

inline void TimerMgr::_BeforeDrive()
{
    ++_driving;
}

inline void TimerMgr::_AfterDrive()
{
    if(--_driving > 0)
        return;

    _driving = 0;

    for(auto iter = _asynDirty.begin(); iter != _asynDirty.end(); )
    {
        auto asynData = *iter;
        auto flag = asynData->_flag;

        if(UNLIKELY(BitUtil::IsSet(flag, AsynOpType::OP_DESTROY)))
        {// 数据销毁
            _Destroy(asynData->_data);
            asynData = NULL;
        }
        else
        {
            // 先执行移除
            if(BitUtil::IsSet(flag, AsynOpType::OP_UNREGISTER))
                _UnRegister(asynData->_data);

            // 最后添加
            if(BitUtil::IsSet(flag, AsynOpType::OP_REGISTER))
                _Register(asynData->_data, asynData->_newPeriod, asynData->_newExpiredTime);

            // 重置数据
            asynData->Reset();
        }
        
        iter = _asynDirty.erase(iter);
    }

    _DoPendings();
}

inline void TimerMgr::_AsynRegister(TimeData *timeData, Int64 newPeriod, Int64 newExpiredTime)
{
    auto asynData = timeData->_asynData;
    asynData->MaskRegister(newExpiredTime, newPeriod);
    timeData->_isScheduing = true;

    _asynDirty.insert(asynData);
}

inline void TimerMgr::_Register(TimeData *timeData, Int64 newPeriod, Int64 newExpiredTime)
{
    timeData->_period = newPeriod;
    timeData->_expiredTime = newExpiredTime;
    timeData->_isScheduing = true;

    _expireQueue.insert(timeData);
}

inline void TimerMgr::_AsynUnRegister(TimeData *timeData)
{
    auto asynData = timeData->_asynData;
    timeData->_isScheduing = false;
    asynData->MaskUnRegister();
    _asynDirty.insert(asynData);
}

inline void TimerMgr::_UnRegister(TimeData *timeData)
{
    auto asynData = timeData->_asynData;
    timeData->_isScheduing = false;
    _expireQueue.erase(timeData);
}

inline void TimerMgr::_Destroy(TimeData *timeData)
{
    timeData->_isScheduing = false;
    _expireQueue.erase(timeData);
    _allTimeData.erase(timeData);
    timeData->Release();
}

inline void TimerMgr::_AsynDestroy(TimeData *timeData)
{
    timeData->_isScheduing = false;
    timeData->_asynData->MaskDestroy();
    _asynDirty.insert(timeData->_asynData);
}

inline bool TimerMgr::_IsInTimerThread()
{
    return _launchThreadId == SystemUtil::GetCurrentThreadId();
}

inline void TimerMgr::_WakeupMgrThread()
{
    if(_wakeupCb)
        _wakeupCb->Invoke();
}

KCP_CPP_END

#endif

