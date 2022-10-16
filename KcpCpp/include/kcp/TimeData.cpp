

#include <pch.h>
#include <kcp/TimeData.h>
#include <kcp/AsynTimeData.h>
#include <kcp/LibTimer.h>
#include <kcp/TimerMgr.h>
#include <kcp/LibTime.h>

KCP_CPP_BEGIN

TimeData::TimeData(Int64 id, LibTimer *timer)
    :_id(id)
    ,_expiredTime(0)
    ,_period(0)
    , _owner(timer)
    ,_isScheduing(false)
    ,_asynData(new AsynTimeData(this))
{

}

TimeData::~TimeData()
{
    delete _asynData;
    _asynData = NULL;
}

LibString TimeData::ToString() const
{
    LibString info;
    const auto &now = LibTime::Now();
    info.AppendFormat("_id=[%lld], _expiredTime=[%lld], _period=[%lld], _owner=[%p], _isScheduing=[%d], left time=[%lld](ms)"
    , _id,  _expiredTime, _period, _owner, _isScheduing, _expiredTime - now.GetMicroTimestamp());

    return info;
}

KCP_CPP_END

