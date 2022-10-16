
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOCKER_HANDLE_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOCKER_HANDLE_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

#ifdef _WIN32
    typedef CRITICAL_SECTION LockerHandle;
#else
    typedef pthread_mutex_t LockerHandle;
#endif

KCP_CPP_END

#endif
