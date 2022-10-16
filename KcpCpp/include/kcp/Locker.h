

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOCKER_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOCKER_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LockerHandle.h>

KCP_CPP_BEGIN

class Locker
{
public:
    Locker();
    virtual ~Locker();

public:
    void Lock();
    void Unlock();
    bool TryLock();

protected:
    LockerHandle _handle;
};

inline void Locker::Lock()
{
#ifdef _WIN32
    ::EnterCriticalSection(&(_handle));
#else
    pthread_mutex_lock(&(_handle));
#endif
}

inline void Locker::Unlock()
{
#ifdef _WIN32
    ::LeaveCriticalSection(&(_handle));
#else
    pthread_mutex_unlock(&(_handle));
#endif
}

inline bool Locker::TryLock()
{
#ifdef _WIN32
    return ::TryEnterCriticalSection(&(_handle));
#else
    return ::pthread_mutex_trylock(&(_handle)) == 0;
    // if (LIKELY(ret == 0))
    // {
    //     return true;
    // }
    // else if(ret != EBUSY)
    // {
    //     perror("trylock error!");
    // }

    // return false;
#endif

}

KCP_CPP_END

#endif
