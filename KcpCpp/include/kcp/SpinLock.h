#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_SPIN_LOCKER_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_SPIN_LOCKER_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

class SpinLock
{
    SpinLock(const SpinLock&) = delete;
    SpinLock(SpinLock&&) = delete;
    SpinLock &operator =(const SpinLock&) = delete;

public:
    SpinLock();
    virtual ~SpinLock();

public:
    void Lock();
    void Unlock();
    bool TryLock();

private:
#ifndef _WIN32
    pthread_spinlock_t _handle;
#else
    CRITICAL_SECTION _handle;
#endif

};

inline SpinLock::~SpinLock()
{
    #ifndef _WIN32
        pthread_spin_destroy(&_handle);
    #else
        ::DeleteCriticalSection(&_handle);
    #endif
}

inline void SpinLock::Lock()
{
    #ifndef _WIN32
        pthread_spin_lock(&_handle);
    #else
        ::EnterCriticalSection(&_handle);
    #endif
}

inline void SpinLock::Unlock()
{
    #ifndef _WIN32
        pthread_spin_unlock(&_handle);
    #else
        ::LeaveCriticalSection(&_handle);
    #endif
}

inline bool SpinLock::TryLock()
{
    #ifndef _WIN32
        return pthread_spin_trylock(&_handle) == 0;
    #else
        return ::TryEnterCriticalSection(&_handle);
    #endif
}

KCP_CPP_END

#endif
