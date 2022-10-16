#include "pch.h"
#include "kcp/common/func.h"
#include "kcp/SpinLock.h"

KCP_CPP_BEGIN

SpinLock &GetConsoleLocker()
{
    static SpinLock s_lck;
    return s_lck;
}

void LockConsole()
{
    GetConsoleLocker().Lock();
}

void UnlockConsole()
{
    GetConsoleLocker().Unlock();
}

KCP_CPP_END