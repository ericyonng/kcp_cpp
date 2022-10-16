
#include <pch.h>
#include <kcp/LibTimer.h>
#include <kcp/TimerMgr.h>
#include <kcp/LibTime.h>


KCP_CPP_BEGIN

LibTimer::LibTimer(TimerMgr *mgr)
    :_mgr(mgr)
    ,_data(mgr->NewTimeData(this))
    ,_timeroutHandler(NULL)
    ,_cancelHandler(NULL)
{
}

LibTimer::~LibTimer()
{
    _mgr->OnTimerDestroy(_data);
    delete _timeroutHandler;
    delete _cancelHandler;
    _data = NULL;
}

void LibTimer::Cancel()
{
    if(LIKELY(_data->_isScheduing))
    {
        _mgr->UnRegister(_data);

        _data->_isScheduing = false;
        if(_cancelHandler)
            _cancelHandler->Invoke(this);
    }
}

void LibTimer::Schedule(Int64 startTime, Int64 milliSecPeriod)
{
    Int64 newPeriod = milliSecPeriod * TimeDefs::MICRO_SECOND_PER_MILLI_SECOND;
    const Int64 expiredTime = startTime + newPeriod;

    // 在定时中先取消定时
    if(_data->_isScheduing)
        Cancel();

    // 重新加入定时
    _mgr->Register(_data, expiredTime, newPeriod);
}

LibString LibTimer::ToString() const
{
    LibString info;
    const auto &now = LibTime::Now();
    info.AppendFormat("_mgr[%p], _data[%s], _timeroutHandler[%p], _cancelHandler[%p]"
    , _mgr, _data->ToString().c_str(), _timeroutHandler, _cancelHandler);

    return info;
}



KCP_CPP_END
