
#include <pch.h>
#include <kcp/TimerMgr.h>
#include <kcp/LibTimer.h>
#include <kcp/TimerDefs.h>
#include <kcp/log.h>

KCP_CPP_BEGIN

TimerMgr::TimerMgr() 
:_launchThreadId(0)
,_driving(0)
,_curTime(0)
,_curMaxId(0)
,_wakeupCb(NULL)
,_pending(NULL)
,_pendingCache(NULL)
{
    
}

TimerMgr::~TimerMgr()
{
    _allTimeData.clear();
    delete _wakeupCb;
    delete _pending;
    delete _pendingCache;
}

void TimerMgr::Launch(IDelegate<void> *wakeupThreadCb)
{
    if(UNLIKELY(_launchThreadId))
    {
        g_Log->Error(LOGFMT_OBJ_TAG("already Launch before _launchThreadId = [%llu], current thread id = [%llu]"), _launchThreadId, SystemUtil::GetCurrentThreadId());
        delete wakeupThreadCb;
        return;
    }

    _launchThreadId = SystemUtil::GetCurrentThreadId();
    _pending = new std::list<BuildTimerInfo *>;
    _pendingCache = new std::list<BuildTimerInfo *>;
    if(_wakeupCb)
        delete _wakeupCb;
    _wakeupCb = wakeupThreadCb;

    // 及时环形处理定时器事件
    if(LIKELY(_wakeupCb))
    {
        _lck.Lock();
        bool isEmpty = _pending->empty();
        _lck.Unlock();
        if(!isEmpty)
            _wakeupCb->Invoke();
    }
}

void TimerMgr::Drive()
{
    _BeforeDrive();

    _curTime = TimeUtil::GetMicroTimestamp();

    for(auto iter = _expireQueue.begin(); iter != _expireQueue.end();)
    {
        // 未过期的为止
        auto timeData = *iter;
        if(timeData->_expiredTime > _curTime)
            break;

        if(timeData->_isScheduing)
        {
            timeData->_owner->OnTimeOut();

            // 如果没有添加则重新计时
            if(timeData->_isScheduing && !timeData->_asynData->IsMaskRegister())
               _AsynRegister(timeData, timeData->_period, timeData->_expiredTime + timeData->_period);
        }

        iter = _expireQueue.erase(iter);
    }

    _AfterDrive();
}

inline void _NewTimer(TimerMgr *mgr, IDelegate<void, LibTimer *> *timeOutCallback, Int64 startTime, Int64 newPeriod, IDelegate<void, LibTimer *, Int32> *completeCallback)
{
    LibTimer *newTimer = new LibTimer(mgr);
    newTimer->SetTimeOutHandler(timeOutCallback);

    newTimer->Schedule(startTime, newPeriod);
    if(completeCallback)
        completeCallback->Invoke(newTimer, Status::Success);
    delete completeCallback;
}

void TimerMgr::AddTimer(IDelegate<void, LibTimer *> *timeOutCallback, Int64 startTime, Int64 newPeriod, IDelegate<void, LibTimer *, Int32> *completeCallback)
{
    // 在timer线程就不需要异步
    if(_IsInTimerThread())
    {
        _NewTimer(this, timeOutCallback, startTime, newPeriod, completeCallback);
        return;
    }

    BuildTimerInfo *buildInfo = new BuildTimerInfo();
    buildInfo->_timeoutDelg = timeOutCallback;
    buildInfo->_completeCallback = completeCallback;
    buildInfo->_startTime = startTime;
    buildInfo->_newPeriod = newPeriod;
    _lck.Lock();
    _pending->push_back(buildInfo);
    _lck.Unlock();

    _WakeupMgrThread();
}

void TimerMgr::_DoPendings()
{
    // swap
    _lck.Lock();
    std::list<BuildTimerInfo *> *t = _pending;
    _pending = _pendingCache;
    _pendingCache = t;
    _lck.Unlock();

    // 注册定时器
    for(auto iter = _pendingCache->begin(); iter != _pendingCache->end();)
    {
        auto buildInfo = *iter;
        _NewTimer(this
                    , buildInfo->_timeoutDelg
                    , buildInfo->_startTime
                    , buildInfo->_newPeriod
                    , buildInfo->_completeCallback);
        delete buildInfo;
        iter = _pendingCache->erase(iter);
    }
}

KCP_CPP_END