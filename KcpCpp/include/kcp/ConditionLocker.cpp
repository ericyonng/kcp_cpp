

#include <pch.h>
#include <kcp/LibTime.h>
#include <kcp/MetaLocker.h>
#include <kcp/ConditionLocker.h>
#include <kcp/util/SystemUtil.h>

KCP_CPP_BEGIN

#ifndef _WIN32
// nsec是时刻中的纳秒部分，注意溢出
static inline void FixAbsTime(UInt64 milliSecond, struct timespec &abstime)
{
    Int64 nanoSecInc = static_cast<Int64>(milliSecond * TimeDefs::NANO_SECOND_PER_MILLI_SECOND);
    clock_gettime(CLOCK_REALTIME, &abstime);
    Int64 curNanoSecPart = static_cast<Int64>(abstime.tv_nsec + nanoSecInc); 

    // 纳秒部分溢出1秒
    abstime.tv_sec += curNanoSecPart / TimeDefs::NANO_SECOND_PER_SECOND;
    curNanoSecPart %= TimeDefs::NANO_SECOND_PER_SECOND;
    abstime.tv_nsec = static_cast<Int64>(curNanoSecPart);
}

static inline void FixAbsTime(UInt64 second, UInt64 microSec, struct timespec &abstime)
{
    // Int64 secInc = second + microSec / TimeDefs::MICRO_SECOND_PER_SECOND;
    // Int64 nanoSecInc = microSec % TimeDefs::MICRO_SECOND_PER_SECOND * 1000;

    Int64 nanoSecInc = microSec * TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
    ::clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += second;
    Int64 curNanoSecPart = static_cast<Int64>(abstime.tv_nsec + nanoSecInc);

    // 纳秒部分溢出1秒
    abstime.tv_sec += curNanoSecPart / TimeDefs::NANO_SECOND_PER_SECOND;
    curNanoSecPart %= TimeDefs::NANO_SECOND_PER_SECOND;
    abstime.tv_nsec = static_cast<Int64>(curNanoSecPart);
}
#endif


ConditionLocker::ConditionLocker()
    :_waitNum{0}
    ,_isSinal{false}

    #ifdef _WIN32
    ,_ev(NULL)
    #endif
{
#ifdef _WIN32
    _ev = CreateEvent(NULL, true, false, NULL);
#else
    int ret = pthread_cond_init(&_ev, NULL);
    if(ret != 0)
    {
        printf("\nret=%d\n", ret);
        perror("cond init error!");
    }
#endif
}

ConditionLocker::~ConditionLocker()
{
#ifdef _WIN32
    if(UNLIKELY(!_ev))
        return;

    if(UNLIKELY(!CloseHandle(_ev)))
    {
        _ev = NULL;
        return;
    }

    _ev = NULL;
#else
    int ret = pthread_cond_destroy(&_ev);
    if(ret != 0)
    {
        printf("\nret=%d\n", ret);
        perror("cond destroy error");
    }
#endif
}

Int32 ConditionLocker::Wait()
{
#ifdef _WIN32
    Int64 waitRet = WAIT_OBJECT_0;
    bool oldSinal = true;
    while(!_isSinal)
    {
        oldSinal = false;
        ++_waitNum;
        Unlock();       
        waitRet = WaitForMultipleObjects(1, &_ev, true, INFINITE);
        Lock();
        --_waitNum;

        // 不论是否被唤醒都重置事件避免消耗
        ResetEvent(_ev);

        if(waitRet == WAIT_TIMEOUT)
        {// 无论是否被唤醒（因为唤醒的时机恰好是超时）超时被唤醒
            _isSinal = false;
            return Status::WaitTimeOut;
        }

        // 出现错误则直接return
        if(!IS_EVENT_SINAL_WAKE_UP(waitRet))
        {
            _isSinal = false;
            return Status::WaitFailure;
        }
    }

    if(oldSinal)
        ResetEvent(_ev);

    _isSinal = false;
    return Status::Success;
#else
    ++_waitNum;
    if(!_isSinal.exchange(false))
    {
        auto ret = pthread_cond_wait(&_ev, &_handle);
        _isSinal = false;
        if(ret != 0)
        {
            --_waitNum;
            printf("\nret=%d\n", ret);
            perror("cConSync::WaitCon -error waitcon");
            _isSinal = false;
            return Status::WaitFailure;
        }
    }
    
    --_waitNum;
    return Status::Success;
#endif

}

Int32 ConditionLocker::TimeWait(UInt64 second, UInt64 microSec)
{
#ifdef _WIN32
    Int64 waitRet = WAIT_OBJECT_0;
    bool oldSinal = true;
    DWORD waitTimeMs = static_cast<DWORD>(second*TimeDefs::MILLI_SECOND_PER_SECOND + microSec / TimeDefs::MICRO_SECOND_PER_MILLI_SECOND);
    while(!_isSinal)
    {
        oldSinal = false;
        ++_waitNum;
        Unlock();
        waitRet = WaitForMultipleObjects(1, &_ev, true, waitTimeMs);
        Lock();
        --_waitNum;

        // 不论是否被唤醒都重置事件避免消耗
        ResetEvent(_ev);

        if(waitRet == WAIT_TIMEOUT)
        {// 无论是否被唤醒（因为唤醒的时机恰好是超时）超时被唤醒
            _isSinal = false;
            return Status::WaitTimeOut;
        }

        // 出现错误则直接return
        if(!IS_EVENT_SINAL_WAKE_UP(waitRet))
        {
            _isSinal = false;
            return Status::WaitFailure;
        }
    }

    if(oldSinal)
        ResetEvent(_ev);

    _isSinal = false;
    return Status::Success;
#else

    // 微妙转换纳秒
    struct timespec abstime; // nsec是时刻中的纳秒部分，注意溢出
    FixAbsTime(second, microSec, abstime);

    ++_waitNum;
    if(!_isSinal.exchange(false))
    {
        // CRYSTAL_TRACE("time wait second[%llu] microSec[%llu] %lld seconds %lld nano sec"
        // , second, microSec, abstime.tv_sec, abstime.tv_nsec);
        int ret = pthread_cond_timedwait(&_ev, &_handle, &abstime);
        _isSinal = false;
        if(ret == ETIMEDOUT)
        {
            --_waitNum;
            return Status::WaitTimeOut;
        }

        if(ret != 0)
        {
            --_waitNum;
            printf("\nret=%d\n", ret);
            perror("pthread cond timewait error");
            return Status::WaitFailure;
        }
    }

    --_waitNum;
    return Status::Success;
#endif
}

Int32 ConditionLocker::TimeWait(UInt64 milliSecond)
{
#ifdef _WIN32
    Int64 waitRet = WAIT_OBJECT_0;
    bool oldSinal = true;
    while(!_isSinal)
    {
        oldSinal = false;
        ++_waitNum;
        Unlock();
        waitRet = WaitForMultipleObjects(1, &_ev, true, static_cast<DWORD>(milliSecond));
        Lock();
        --_waitNum;

        // 不论是否被唤醒都重置事件避免消耗
        ResetEvent(_ev);

        if(waitRet == WAIT_TIMEOUT)
        {// 无论是否被唤醒（因为唤醒的时机恰好是超时）超时被唤醒
            _isSinal = false;
            return Status::WaitTimeOut;
        }

        // 出现错误则直接return
        if(!IS_EVENT_SINAL_WAKE_UP(waitRet))
        {
            _isSinal = false;
            return Status::WaitFailure;
        }
    }

    if(oldSinal)
        ResetEvent(_ev);

    _isSinal = false;
    return Status::Success;
#else
    // nsec是时刻中的纳秒部分，注意溢出
    struct timespec abstime; 
    FixAbsTime(milliSecond, abstime);

    ++_waitNum;
    if(!_isSinal.exchange(false))
    {
        // CRYSTAL_TRACE("time wait milliSecond[%llu] %lld seconds %lld nano sec", milliSecond, abstime.tv_sec, abstime.tv_nsec);
        int ret = pthread_cond_timedwait(&_ev, &_handle, &abstime);
        _isSinal = false;
        if(ret == ETIMEDOUT)
        {
            --_waitNum;
            return Status::WaitTimeOut;
        }

        if(ret != 0)
        {
            --_waitNum;
            printf("\nret=%d\n", ret);
            perror("pthread cond timewait error");
            return Status::WaitFailure;
        }
    }

    --_waitNum;
    return Status::Success;
#endif
}


bool ConditionLocker::Sinal()
{
#ifdef _WIN32
    if(!_isSinal.exchange(true))
    {// 至少有一个waiter或者没有waiter

        // 保证不丢失信号
        Lock();
        _isSinal = true;
        if(_waitNum > 0)
            SetEvent(_ev);
        Unlock();
    }

    return _isSinal.load();
#else
    if(!_isSinal.exchange(true))
    {// 保证只有一个Sinal调用者进入 , sinal 为false说明,wait那边把sinal置为false,或者没有waiter

        // 有waiter情况下 _waitNum必定大于0
        if(LIKELY(_waitNum > 0))
        {
            Lock();
            int ret = pthread_cond_signal(&_ev);
            if(ret != 0)
            {
                printf("\nret=%d\n", ret);
                perror("signal fail\n");
                Unlock();
                return false;
            }
            Unlock();
        }
    }

    return _isSinal.load();
#endif
}

void ConditionLocker::Broadcast()
{
#ifdef _WIN32
    bool isSinal = false;
    while(_waitNum > 0)
        Sinal();

//     if(LIKELY(isSinal))
//     {
//         _isSinal = true;
//         ResetEvent(_event.load());
//     }
#else
    Lock();
    _isSinal = true;
    Unlock();

    int ret = pthread_cond_broadcast(&_ev);
    if(ret != 0)
    {
        printf("\nret=%d\n", ret);
        perror("cond broadcast error");
        return;
    }

#endif
}

void ConditionLocker::ResetSinal()
{
#ifdef _WIN32
    ResetEvent(_ev);
#endif
}

KCP_CPP_END
