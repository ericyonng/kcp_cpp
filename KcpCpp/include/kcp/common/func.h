#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_FUNC_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_FUNC_H__

#pragma once

#include "kcp/common/macro.h"

template<typename T>
inline T *LibCast(void *ptr)
{
    return reinterpret_cast<T *>(ptr);
}

template<typename T>
inline const T *LibCast(const void *ptr)
{
    return reinterpret_cast<const T *>(ptr);
}

KCP_CPP_BEGIN

class SpinLock;
SpinLock &GetConsoleLocker();
void LockConsole();
void UnlockConsole();

KCP_CPP_END

#endif
