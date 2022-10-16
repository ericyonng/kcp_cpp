
#include <pch.h>
#include <kcp/Locker.h>
#include <kcp/util/CountUtil.h>

KCP_CPP_BEGIN

Locker::Locker()
{
#ifdef _WIN32
    static const UInt32 maxDwordCount1 = CountUtil::Count1InBinary((std::numeric_limits<DWORD>::max)());
#endif

    ::memset(&_handle, 0, sizeof(_handle));
    
#ifdef _WIN32
    // 自旋次数（最高位置1）
    DWORD spinCnt = static_cast<DWORD>(SPINNING_COUNT);
    spinCnt |= static_cast<DWORD>(1 << (maxDwordCount1 - 1));

    // 创建自旋锁，避免线程频繁挂起
    if(!::InitializeCriticalSectionAndSpinCount(&(_handle), spinCnt))
        printf("MetaLocker create spinlock fail spinCnt[%lu]", spinCnt);
#else
    auto ret = pthread_mutex_init(&(_handle), NULL);
    if(ret != 0)
        perror("\nMetaLocker create fail");

#endif // _WIN32
}

Locker::~Locker()
{
#ifndef _WIN32
    auto ret = pthread_mutex_destroy(&_handle);
    if(ret != 0)
        perror("pthread_mutex_destroy fail");

#else
    ::DeleteCriticalSection(&_handle);
#endif
}

KCP_CPP_END

