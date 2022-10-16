
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_CONDITION_LOCKER_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_CONDITION_LOCKER_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/Locker.h>

KCP_CPP_BEGIN

class ConditionLocker : public Locker
{
public:
    ConditionLocker();
    virtual ~ConditionLocker();

public:
    Int32 Wait();
    Int32 TimeWait(UInt64 second, UInt64 microSec);
    Int32 TimeWait(UInt64 milliSecond);

    bool Sinal();   // 无需加锁 保证信号不丢失
    bool HasWaiter();
    void Broadcast();   // 无需加锁 保证信号不丢失
    void ResetSinal();
    void ResetSinalFlag();
    bool IsSinal() const;

private:
    std::atomic<Int64> _waitNum;
    std::atomic_bool _isSinal;

#ifdef _WIN32
    HANDLE _ev;
#else
    pthread_cond_t _ev;
#endif

};

inline bool ConditionLocker::HasWaiter()
{
    return _waitNum > 0;
}

inline void ConditionLocker::ResetSinalFlag()
{
    _isSinal = false;
}

inline bool ConditionLocker::IsSinal() const
{
    return _isSinal;
}

KCP_CPP_END

#endif